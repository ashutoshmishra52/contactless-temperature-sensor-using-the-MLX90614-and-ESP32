// ESP32 + MLX90614 + SSD1306
// Clean UI + LED alert (GPIO2) turns ON when temp >= 60°C
// Libraries required:
//   Adafruit_MLX90614
//   Adafruit_SSD1306
//   Adafruit_GFX

#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// I2C pins for ESP32
#define SDA_PIN 21
#define SCL_PIN 22

// OLED config
#define SCREEN_W 128
#define SCREEN_H 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_W, SCREEN_H, &Wire, OLED_RESET);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

// Timing & smoothing
const unsigned long REFRESH_MS = 400;
const float EMA_ALPHA = 0.25f;

// LED alert settings
const int LED_PIN = 2;                 // onboard LED (common on many ESP32 dev boards)
const float VERY_HOT_THRESHOLD_C = 60.0f; // LED turns ON at or above this temp

// Thermometer bar geometry (right side)
const int BAR_X = 92;
const int BAR_Y = 14;
const int BAR_W = 30;
const int BAR_H = 40;
const float BAR_MIN_T = 0.0f;   // bar lower scale
const float BAR_MAX_T = 80.0f;  // bar upper scale

// Globals
unsigned long lastUpdate = 0;
float smoothC = NAN;

// Helpers
float clampf(float v, float lo, float hi) {
  if (v < lo) return lo;
  if (v > hi) return hi;
  return v;
}
float cToF(float c) { return isnan(c) ? NAN : (c * 9.0f / 5.0f + 32.0f); }

// Draw right-side thermometer bar (no overlap)
void drawBar(float tempC) {
  display.drawRoundRect(BAR_X, BAR_Y, BAR_W, BAR_H, 3, SSD1306_WHITE);
  // bulb
  int bulbX = BAR_X + BAR_W / 2;
  int bulbY = BAR_Y + BAR_H + 6;
  display.fillCircle(bulbX, bulbY, 5, SSD1306_WHITE);

  // map temperature to fill
  float frac = (tempC - BAR_MIN_T) / (BAR_MAX_T - BAR_MIN_T);
  frac = clampf(frac, 0.0f, 1.0f);
  int fillH = (int)(frac * BAR_H + 0.5f);
  int fillTop = BAR_Y + (BAR_H - fillH);

  if (fillH > 0) display.fillRect(BAR_X + 2, fillTop, BAR_W - 4, fillH, SSD1306_WHITE);

  // subtle stripes for segment look
  for (int y = BAR_Y; y < BAR_Y + BAR_H; y += 6) {
    display.drawFastHLine(BAR_X + 3, y + 2, BAR_W - 6, SSD1306_BLACK);
  }
}

// Render the full screen
void renderUI(float C, float F, float roomC) {
  display.clearDisplay();

  // header
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Temperature Sensor");
  display.drawFastHLine(0, 11, SCREEN_W, SSD1306_WHITE);

  // big Celsius value (left)
  display.setTextSize(3);
  display.setCursor(4, 16);
  if (!isnan(C)) display.printf("%.1f", C);
  else display.print("--.-");

  // small "C" label (fixed position to avoid overlap)
  display.setTextSize(1);
  display.setCursor(75, 22);
  display.print("C");

  // Fahrenheit (small, below big number)
  display.setTextSize(1);
  display.setCursor(4, 46);
  if (!isnan(F)) display.printf("%.1f F", F);
  else display.print("--.- F");

  // draw right-side bar
  drawBar(C);

  // room (ambient) temp bottom-left
  display.setCursor(4, SCREEN_H - 10);
  if (!isnan(roomC)) display.printf("Room: %.1f C", roomC);
  else display.print("Room: --.- C");

  display.display();
}

void setup() {
  // LED pin: ensure defined output and start LOW
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.begin(115200);
  delay(50);

  Wire.begin(SDA_PIN, SCL_PIN);

  // init OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    while (1) delay(10);
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // init MLX90614
  if (!mlx.begin()) {
    Serial.println("MLX90614 not found. Check wiring!");
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("MLX90614 not found!");
    display.display();
    while (1) delay(500);
  }

  // seed smoothing with initial reading if available
  float first = mlx.readObjectTempC();
  if (!(first < -100.0)) smoothC = first;
}

void loop() {
  unsigned long now = millis();
  if (now - lastUpdate < REFRESH_MS) return;
  lastUpdate = now;

  float rawC = mlx.readObjectTempC();
  float roomC = mlx.readAmbientTempC();

  if (rawC < -100.0) rawC = NAN;
  if (roomC < -100.0) roomC = NAN;

  // EMA smoothing
  if (!isnan(rawC)) {
    if (isnan(smoothC)) smoothC = rawC;
    else smoothC = EMA_ALPHA * rawC + (1.0f - EMA_ALPHA) * smoothC;
  }

  float fVal = cToF(smoothC);

  // LED logic: ON when temp >= VERY_HOT_THRESHOLD_C
  if (!isnan(smoothC) && smoothC >= VERY_HOT_THRESHOLD_C) digitalWrite(LED_PIN, HIGH);
  else digitalWrite(LED_PIN, LOW);

  // render display
  renderUI(smoothC, fVal, roomC);
}
