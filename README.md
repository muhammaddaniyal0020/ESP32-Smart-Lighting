# ESP32-Based Smart Lighting Control System

A cost-effective, IoT-based smart lighting control system using ESP32 microcontroller and MQTT protocol with Flutter mobile application.

## 🚀 Features

- **Individual Light Control**: Control 4 separate lights independently
- **Master Controls**: Turn all lights ON/OFF with one tap
- **Real-time Feedback**: Live status updates of all lights
- **Cross-platform Mobile App**: Built with Flutter (Android & iOS)
- **MQTT Protocol**: Lightweight and efficient communication
- **Cost-Effective**: Total system cost under $30

## 🛠️ Hardware Components

| Component | Model | Quantity |
|-----------|-------|----------|
| ESP32-S3 | WROOM-1-N16R8 | 1 |
| 4-Channel Relay Module | SRD-05VDC-SL-C | 1 |
| LED Bulbs | Standard 220V AC | 4 |
| Bulb Holders | E27 Socket | 4 |
| Power Supply | 5V 2A | 1 |
| Jumper Wires | F-F 20cm | 15 |
| AC Cable | 3-core 18 AWG | 5m |
| Junction Box | Plastic | 1 |

## 📋 Pin Configuration

| ESP32 Pin | Relay Module | Wire Color | Description |
|-----------|--------------|------------|-------------|
| 5V | VCC  | Red   | Power for Relay Coils |
| GND | GND | Black | Common Ground |
| GPIO13    | IN1   | Yellow | Light 1 (Living Room) |
| GPIO12    | IN2   | Green  | Light 2 (Bedroom) |
| GPIO14    | IN3   | Blue   | Light 3 (Kitchen) |
| GPIO27    | IN4   | White  | Light 4 (Study Room) |

## 📡 MQTT Topics

| Topic | Direction | Payload | Purpose |
|-------|-----------|---------|---------|
| home/light/1/command | Mobile → ESP32 | ON/OFF | Control Light 1 |
| home/light/2/command | Mobile → ESP32 | ON/OFF | Control Light 2 |
| home/light/3/command | Mobile → ESP32 | ON/OFF | Control Light 3 |
| home/light/4/command | Mobile → ESP32 | ON/OFF | Control Light 4 |
| home/all/command    | Mobile → ESP32 | ON/OFF | Control All Lights |
| home/light/1/status | ESP32 → Mobile | ON/OFF | Light 1 Status |
| home/light/2/status | ESP32 → Mobile | ON/OFF | Light 2 Status |
| home/light/3/status | ESP32 → Mobile | ON/OFF | Light 3 Status |
| home/light/4/status | ESP32 → Mobile | ON/OFF | Light 4 Status |

## 📱 Mobile App Screenshots

[Add screenshots here]

## 🔧 Installation

### ESP32 Setup

1. Install Arduino IDE
2. Install ESP32 board support
3. Install PubSubClient library
4. Update Wi-Fi credentials in code
5. Upload code to ESP32

### Flutter App Setup

1. Install Flutter SDK
2. Clone this repository
3. Run `flutter pub get`
4. Build APK: `flutter build apk`
5. Install on mobile device

## 📊 Performance Metrics

| Metric | Value |
|--------|-------|
| Average Response Time | 225 ms |
| MQTT Packet Size | 15-50 bytes |
| Wi-Fi Range | 30 meters |
| Connection Stability | >99% uptime |

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 👨‍💻 Authors

- **Muhammad Daniyal** - Roll No. 62413
- **Bushra Javed** - Roll No. 62313
- **Dua Bukhari** - Roll No. 62301

## 🙏 Acknowledgments

- **Madam Shehr Bano** - Supervisor
- **Madam Nosheen Jelani** - Co-Supervisor
- **Institute of Computational Intelligence, Gomal University**

## 📚 References

1. Espressif Systems. (2024). "ESP32 Technical Reference Manual"
2. HiveMQ. (2024). "MQTT Essentials"
3. OASIS MQTT Technical Committee. (2019). "MQTT Version 5.0 Specification"
