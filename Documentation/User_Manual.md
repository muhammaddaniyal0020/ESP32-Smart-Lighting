# User Manual

## C.1 System Installation

### 1. Hardware Setup:
- Connect ESP32 to relay module according to pin mapping
- Connect relay outputs to LED bulbs
- Connect power supply to ESP32
- Place all components in junction box

### 2. ESP32 Programming:
- Install Arduino IDE
- Install ESP32 board support
- Install PubSubClient library
- Update Wi-Fi credentials in code
- Upload code to ESP32

### 3. Mobile App Installation:
- Build APK using Flutter
- Transfer APK to mobile device
- Install the application

## C.2 System Operation

### 1. Starting the System:
- Power on ESP32
- Wait for Wi-Fi connection (LED will indicate)
- Open mobile app
- Verify "Online" status in app

### 2. Controlling Lights:
- Tap any light card to toggle ON/OFF
- Use "ALL ON" button to turn on all lights
- Use "ALL OFF" button to turn off all lights
- Observe real-time status changes

## C.3 Troubleshooting

| Issue | Solution |
|-------|----------|
| App shows Offline | Check ESP32 power, Wi-Fi connection |
| No response from lights | Check relay connections, AC power |
| Intermittent operation | Check Wi-Fi signal strength |
| App not connecting | Verify phone on same network |