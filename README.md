# ⏱️ ESP32 Auto Trigger System with Contactless Temperature Sensor

This project is an ESP32-based automatic trigger system using a timer-based mechanism.
It uses two servo motors, an OLED display, and a contactless temperature sensor (MLX90614).
The system works fully automatically without any manual input.

---

## 🔧 Components Used

- ESP32 (Main Controller)
- 2 × Servo Motors  
  - Servo 1: Horizontal / alignment movement  
  - Servo 2: Trigger / firing action  
- OLED Display (I2C)
- MLX90614 Contactless Temperature Sensor
- External 5V power supply (for servos)

---

## ⚙️ How the Auto Trigger Mechanism Works (Timer-Based)

The ESP32 uses a non-blocking timer based on the `millis()` function instead of `delay()`.

### Working Flow
1. On power ON, both servos move to their default position
2. OLED displays system status and timer
3. ESP32 continuously checks the timer condition
4. When the defined interval is completed:
   - Servo 1 aligns to a fixed angle
   - Servo 2 performs the trigger action
5. Servos reset automatically
6. Timer restarts and the cycle repeats

```

Start → Timer Running → Time Completed → Servo Trigger
→ Reset Position → Timer Restart → Repeat

```

---

## 🌡️ Contactless Temperature Sensor (MLX90614)

- Measures temperature without touching the object
- Uses infrared sensing technology
- Communicates with ESP32 via I2C
- Real-time temperature is shown on the OLED display
- Can be used to activate alerts or actions when temperature crosses a set threshold

---

## 📌 Pin Mapping (ESP32)

### Servo Motors
| Component | ESP32 Pin | Description |
|---------|-----------|-------------|
| Servo 1 | GPIO 26   | Horizontal movement |
| Servo 2 | GPIO 27   | Trigger / firing action |

⚠️ Servos must be powered using an external 5V supply.  
ESP32 GND and servo power GND must be common.

---

### OLED Display (I2C)
| OLED Pin | ESP32 Pin |
|--------|-----------|
| VCC    | 3.3V |
| GND    | GND |
| SDA    | GPIO 21 |
| SCL    | GPIO 22 |

---

### MLX90614 Temperature Sensor (I2C)
| MLX90614 Pin | ESP32 Pin |
|-------------|-----------|
| VCC / VIN   | 3.3V |
| GND         | GND |
| SDA         | GPIO 21 |
| SCL         | GPIO 22 |

OLED and MLX90614 share the same I2C bus.

Default I2C Addresses:
- OLED: 0x3C
- MLX90614: 0x5A

---

## 🧠 Key Features

- Fully automatic operation
- Timer-based logic using `millis()`
- Smooth and reliable servo control
- Real-time OLED feedback
- Contactless temperature measurement
- Easy to modify trigger interval and temperature thresholds

---

## 🛠️ Programming Language

- C++ (Arduino framework)
- `.ino` sketch file
- Uses Arduino libraries for Servo, I2C, and OLED

---

## 🚀 Applications

- Educational automation projects
- Robotics trigger mechanisms
- Servo-based control systems
- IoT timed actions
- Contactless temperature monitoring

---

## 📌 Notes

- Do not power servo motors directly from ESP32 5V pin
- Always use PWM-capable GPIOs for servos
- Ensure common ground between all components

