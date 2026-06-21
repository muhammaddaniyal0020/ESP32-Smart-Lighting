# Wiring Diagrams

## Low Voltage Connections

| ESP32 Pin | Relay Module | Wire Color |
|-----------|--------------|------------|
| 5V | VCC | Red |
| GND | GND | Black |
| GPIO13 | IN1 | Yellow |
| GPIO12 | IN2 | Green |
| GPIO14 | IN3 | Blue |
| GPIO27 | IN4 | White |

## High Voltage Connections

Control the bulbs with individual NO (Normally Open) relay contacts while powering them in parallel.

### Connection Instructions:

1. **AC Live (Mains)** → Connect to all COM terminals (COM1, COM2, COM3, COM4)
2. **Channel 1 Output (NO1)** → Light 1 (Live Wire)
3. **Channel 2 Output (NO2)** → Light 2 (Live Wire)
4. **Channel 3 Output (NO3)** → Light 3 (Live Wire)
5. **Channel 4 Output (NO4)** → Light 4 (Live Wire)
6. **AC Neutral (Mains)** → Connect all bulb neutral wires directly to AC Neutral

### Color Coding Reference

| Wire Color | Side | Purpose |
|------------|------|---------|
| Red | Low Voltage | 5V Power |
| Black | Low Voltage | Ground |
| Yellow | Low Voltage | Light 1 Control |
| Green | Low Voltage | Light 2 Control |
| Blue | Low Voltage | Light 3 Control |
| White | Low Voltage | Light 4 Control |
| Brown | High Voltage | AC Live / Bulb Live |
| Blue | High Voltage | AC Neutral |