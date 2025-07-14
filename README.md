# 🌱 AGRIBOT - Smart Farming Rover 🤖🚜

**AGRIBOT** is a multifunctional, IoT-enabled farming rover designed to automate various agricultural tasks like seed planting, soil preparation, and environmental monitoring using sensors. It is powered by an **ESP32 microcontroller** and accessible via a web-based control panel.

---

## 🚀 Features

- 📶 Wi-Fi Controlled Rover
- 🌡️ Real-time temperature and pressure monitoring (BMP180 sensor)
- 📏 Ultrasonic distance sensor for obstacle detection
- 💧 Water pump control (relay-based)
- ⚙️ Seed-gear motor control (relay-based)
- 🕹️ Manual movement control: Forward, Backward, Left, Right
- 📱 Mobile/PC responsive web interface

---

## 🧠 Microcontroller & Sensors

| Component            | Description                             |
|----------------------|-----------------------------------------|
| ESP32                | Main microcontroller with WiFi          |
| BMP180 / BMP085      | Barometric pressure + Temperature sensor|
| HC-SR04              | Ultrasonic distance sensor              |
| 2-Channel Relay      | Controls Gear Motor & Pump Motor        |
| L298N or H-Bridge    | Motor driving circuit (2 motors)        |
| DC Gear Motors (x2)  | Rover drive motors                      |
| Submersible Pump     | For irrigation                          |

---

## 🧰 Wiring Table

| ESP32 Pin | Connected To              | Function                 |
|----------:|---------------------------|--------------------------|
| GPIO 27   | L298N IN1                 | Left Motor Forward       |
| GPIO 26   | L298N IN2                 | Left Motor Backward      |
| GPIO 14   | L298N ENA (PWM)           | Speed control (Left)     |
| GPIO 25   | L298N IN3                 | Right Motor Forward      |
| GPIO 33   | L298N IN4                 | Right Motor Backward     |
| GPIO 32   | L298N ENB (PWM)           | Speed control (Right)    |
| GPIO 5    | HC-SR04 Trig              | Ultrasonic Trigger       |
| GPIO 18   | HC-SR04 Echo              | Ultrasonic Echo          |
| GPIO 19   | Relay IN1                 | Gear Motor Control       |
| GPIO 23   | Relay IN2                 | Water Pump Control       |
| GPIO 2    | Onboard LED               | WiFi Indicator / Status  |
| GPIO 21/22| I2C (SCL/SDA)             | BMP180 Sensor            |

> ⚠️ Make sure to use external 5V power supply for motors and relays!

---

## 🛠️ Installation

1. **Clone this repo**:
    ```bash
    https://github.com/AdiWork933/AGRIBOT-SMART-FARMING-ROVER.git
    cd agribot-rover
    ```

2. **Libraries Required (Install via Library Manager)**:
    - `WiFi.h`
    - `WebServer.h`
    - `Adafruit BMP085 Unified`
    - `Wire.h`

3. **Open `FREEbot_V1.ino` in Arduino IDE**
4. **Select Board**: ESP32 Dev Module  
5. **Fill in WiFi Credentials**:
    ```cpp
    const char* ssid = "YOUR_WIFI_NAME";
    const char* password = "YOUR_WIFI_PASSWORD";
    ```

6. **Upload the code**
7. **Open Serial Monitor @ 115200**  
   It will show the IP address to access the web interface.

---

## 🌐 Web Control Panel

Once ESP32 is connected to WiFi, visit the IP address shown on Serial Monitor (e.g., `http://192.168.1.105`):

### Web Features:
- 📊 View:
  - Distance from obstacle
  - Status (object detected or not)
  - Temperature
  - Pressure
- 🎮 Control:
  - Speed using slider
  - Direction: Forward, Backward, Left, Right, Stop
  - Gear Motor ON/OFF
  - Pump Motor ON/OFF

Interface auto-refreshes every 2 seconds to show real-time sensor data.

---

## 📸 Screenshots

> (You can include interface screenshots here)

---

## 🧪 Future Improvements

- Add soil moisture sensor for better irrigation decisions  
- Implement GPS navigation  
- Integrate voice commands or Bluetooth fallback  
- Add camera + AI for smart crop detection  

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## 🙌 Contribution

Pull requests are welcome! Feel free to fork and modify the project for your own use cases.

---

## 🔗 Credits

- Developed by: **[Aditya Choudhary, Sayantan Das, ]**
- Based on ESP32, Adafruit BMP180, HC-SR04, and Relay Modules

---

