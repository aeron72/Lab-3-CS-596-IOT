# Lab-3-CS-596-IOT

Members(Solo): Aeron Flores

Video Demo Link: https://youtube.com/shorts/TFic_ymkoD0?feature=share

Top Down Image
![TempMoist](https://github.com/user-attachments/assets/465c6788-7763-4d13-b9dc-94fc7fe452cd)


This project uses an ESP32 development board with a DHT20 sensor to read real-time temperature and humidity data, and sends the data to a Flask server hosted on an AWS EC2 instance. The server logs and displays the data on a web dashboard, updating every few seconds.

---



## 🧠 Components

- **ESP32 board** (TTGO LoRa32 used in `platformio.ini`)
- **DFRobot DHT20 sensor**
- **AWS EC2 instance** running a Python Flask server
- **Real-time web dashboard** showing last 20 sensor readings

---

## 📁 File Structure

```
project/
│
├── main.cpp           # ESP32 firmware (reads DHT20 + sends to server)
├── platformio.ini     # PlatformIO config for ESP32 build
├── server.py          # Flask server for receiving and displaying sensor data
└── README.md          # You're here!
```

---

## 🚀 ESP32 Firmware (`main.cpp`)

The firmware:

- Connects to Wi-Fi
- Initializes the DHT20 sensor
- Reads temperature and humidity every 10 seconds
- Sends data to the server as HTTP GET requests
- Handles Wi-Fi reconnects gracefully

📡 Example request sent:
```
GET http://<your-ec2-ip>:5000/update?temp=24.8&humidity=52.3
```

---

## ⚙️ PlatformIO Configuration (`platformio.ini`)

This file sets up the ESP32 build environment using the Arduino framework and required libraries.

```ini
[env:ttgo-lora32-v1]
platform = espressif32
board = ttgo-lora32-v1
framework = arduino
monitor_speed = 115200

lib_deps = 
  amcewen/HttpClient@^2.2.0
  dfrobot/DFRobot_DHT20@^1.0.0
```

📝 Make sure to replace your Wi-Fi credentials and server IP in `main.cpp`.

---

## 🌐 Flask Server (`server.py`)

A lightweight Python Flask server that:

- Listens for `/update` requests with `temp` and `humidity` values
- Stores and prints logs with timestamps
- Serves a real-time dashboard that auto-refreshes every 2 seconds
- Uses a background thread to simulate serial-style live logging

💻 Example dashboard output:
```
ESP32 Sensor Data
[20/Mar/2025 14:55:01] Temperature: 24.80°C, Humidity: 52.30%
...
```

🛡️ Make sure your EC2 instance allows inbound traffic on port `5000`.

---

## ✅ Getting Started

### 1. Set Up the Flask Server

On your EC2 instance:

```bash
sudo apt update
sudo apt install python3-pip
pip3 install flask
python3 server.py
```

Make sure port `5000` is open in your EC2 Security Group.

### 2. Flash the ESP32

On your development machine:

```bash
# Install PlatformIO if needed
pip install platformio

# Build & upload firmware
platformio run --target upload
```

Check the serial monitor to verify Wi-Fi connection and data transmission.

---
