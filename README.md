# ESP32-H2-DevKitM-1 + Zigbee2MQTT
<div align="center">
    <img alt="Erase Flash" src="https://github.com/user-attachments/assets/c77d54e3-c5ce-4278-bef6-b1f04a8e2a4d" />
    <img alt="Erase Flash" src="https://github.com/user-attachments/assets/bbb3fd09-c9fe-449e-9a9b-129e5e6c79af" />
</div>

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

## Potential problem
Sometimes the zigbee stack won't start. Try "Erase Flash" to fix it.

<div align="center">
    <img width="480" height="333" alt="Erase Flash" src="https://github.com/user-attachments/assets/6b7483c6-b706-47a6-856c-3aee93df1de2" />
</div>

## Related Projects
* [Zigbee2MQTT](https://www.zigbee2mqtt.io/)






