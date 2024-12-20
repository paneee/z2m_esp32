# ESP32-H2-DevKitM-1 + Zigbee2MQTT
![esp32-h2-devkitm-1](https://github.com/user-attachments/assets/c77d54e3-c5ce-4278-bef6-b1f04a8e2a4d)
![zigbee](https://github.com/user-attachments/assets/bbb3fd09-c9fe-449e-9a9b-129e5e6c79af)


This example show reading inputs and writing outputs for the ESP32-H2 processor connected to the zigbee network in the zigbee2MQTT application using custom clusters. Contains the source code for the processor and the converter code for zigbee2mqtt written in javascript.

## Supported Targets 
  - ESP32-C6
  - ESP32-H2

## Hardware Platform
  - ESP32-H2-DevKitM-1

## Software Platform
  - ESP-IDF v5.4  
  - Zigbee2MQTT  
  - Visual Studio Code + ESP-IDF extension  

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

## Operating Modes
The device has two operating modes depending on the setting:  
```c
#define MODE_RANDOM_OR_REWRITE true
```
in z2m.h file set to true or false.
  - true  - mode of writing random values ​​to the outputs
  - false - mode of rewriting outputs from inputs

## Demonstration
https://github.com/user-attachments/assets/d844e484-898a-47dd-9222-8dc0411be572

## Related Projects
* [Zigbee2MQTT](https://www.zigbee2mqtt.io/)





