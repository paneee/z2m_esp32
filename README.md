| Supported Targets | ESP32-C6 | ESP32-H2 |
| ----------------- | -------- | -------- |

# ESP32-H2-DevKitM-1 + Zigbee2MQTT + VSCode

## Description
This example show reading inputs and writing outputs for the ESP32-H2 processor connected to the zigbee network in the zigbee2MQTT application using custom clusters.
<br/>
<br/>

## Hardware platform
  - ESP32-H2-DevKitM-1
<br/>

## Software platform
  - ESP-IDF v5.4  
  - Zigbee2MQTT  
  - Visual Studio Code + ESP-IDF extension  
<br/>

## Procedure
| Component            | Action                                                               |
|----------------------|----------------------------------------------------------------------|
| Zigbee2MQTT          | Add external converter (file ../z2m_converter/ESP32_H2.js)           |
| VSCode               | Install VSCode, add EXTENSION ESP-IDF, install ESP-IDF environment   |
| VSCode               | Open project                                                         |
| VSCode               | Select platform (ESP32H2 or ESP32H2) and COM port                    |
| VSCode               | Build the project                                                    |
| VSCode               | Flash device                                                         |
| VSCode               | Monitor device                                                       |
| Zigbee2MQTT          | Permit join devices                                                  |
| ESP32-H2-DevKitM-1   | Reset device                                                         |
| Zigbee2MQTT          | After correctly adding it in zigbee2mqtt, we can see the operation of the device |
<br/>

## Operating modes
The device has two operating modes depending on the setting:
#define MODE_RANDOM_OR_REWRITE (z2m.h file), set to true or false.
  - True  - mode of writing random values ​​to the outputs
  - False - mode of rewriting outputs from inputs
<br/>

## Demonstration
https://github.com/user-attachments/assets/4b21cd15-e4d4-402e-85a5-228b1e0edc12



