# Twomes boiler BASE firmware

For the associated hardware design files for the Twomes boiler BASE and enclosure and tips and instructions how to produce and assemble the hardware, please see the [twomes-boiler-base-hardware](https://github.com/energietransitie/twomes-boiler-base-hardware) repository. 

## Table of contents
* [General info](#general-info)
* [Measurements](#measurements)
* [Deploying](#deploying)
* [Developing](#developing) 
* [Supported devices](#supported-devices)
* [Features](#features)
* [Status](#status)
* [License](#license)
* [Credits](#credits)

## General info
The Twomes Presence Detector primarily serves as an example and template repository how to create firmware that uses the [twomes-generic-esp-firmware](https://github.com/energietransitie/twomes-generic-esp-firmware) library. Nevertheless, it can be used in  measurement campaigns that require occupancy detection using Bluetooth name requests.

## Measurements

A Twomes boiler BASE device, in addition to [generic data sent by any Twomes measurement device](https://github.com/energietransitie/twomes-generic-esp-firmware#readme), sends data about the following properties via the [Twomes API](https://github.com/energietransitie/twomes-backoffice-api) to a Twomes server:

|  Sensor | OpenTherm ID | Part |         Property         | Unit | Printf format | Measurement interval [h:mm:ss] |                 Description                |
|:-------:|:------------:|:----:|:------------------------:|:----:|:-------------:|:------------------------------:|:------------------------------------------:|
|         | 0            | LB/3 | isBoilerFlameOn          |      | 0/1           | 0:00:30                        | STATUS /Flame status                       |
|         | 0            | LB/1 | isCentralHeatingModeOn   |      | 0/1           | 0:00:30                        | STATUS/CH mode                             |
|         | 0            | LB/2 | isDomesticHotWaterModeOn |      | 0/1           | 0:00:30                        | STATUS/DHW mode                            |
|         | 14           |      | maxModulationLevel       | %    | %d            | 0:00:30                        | CAPACITY SETTING                           |
|         | 15           | HB   | maxBoilerCap             | kW   | %d            | 0:00:30                        | MAX CAPACITY                               |
|         | 15           | LB   | minModulationLevel       | %    | %d            | 0:00:30                        | MIN-MOD-LEVEL                              |
|         | 16           |      | roomSetpointTemp         | °C   | %.2f          | 0:05:00                        | ROOM SETPOINT                              |
|         | 17           |      | relativeModulationLevel  | %    | %d            | 0:00:30                        | RELATIVE MODULATION LEVEL                  |
|         | 24           |      | roomTemp                 | °C   | %.2f          | 0:05:00                        | ROOM TEMPERATURE                           |
|         | 57           |      | boilerMaxSupplyTemp      | °C   | %.2f          | 0:05:00                        | MAX CH WATER SETPOINT                      |
|         | 25           |      | boilerSupplyTemp         | °C   | %.2f          | 0:00:10                        | BOILER WATER TEMP.                         |
|         | 28           |      | boilerReturnTemp         | °C   | %.2f          | 0:00:10                        | RETURN WATER TEMPERATURE                   |
| DS18b20 |              |      | boilerTemp1              | °C   | %.1f          | 0:00:10                        | temperature of hydronic supply/return pipe |
| DS18b20 |              |      | boilerTemp2              | °C   | %.1f          | 0:00:10                        | temperature of hydronic supply/return pipe |

## Deploying
To deploy this software, see the [deploying section in the twomes-generic-esp-firmware library documentation](https://www.energietransitiewindesheim.nl/twomes-generic-esp-firmware/deploying/prerequisites/). The firmware needed can be found as a [release from this repository](https://github.com/energietransitie/twomes-presence-detector/releases).

## Developing
To develop software for, or based on this software, see the [developing section in the twomes-generic-esp-firmware library documentation](https://www.energietransitiewindesheim.nl/twomes-generic-esp-firmware/starting/prerequisites/)

## Supported devices
This example was tested on:
- [M5Stack CoreInk](https://github.com/m5stack/M5-CoreInk)

## Features
The example features all generic firmware tasks.

Ready:
* awesome feature 1;
* awesome feature 2;
* awesome feature 3.

To-do:
* wow improvement to be done 1;
* wow improvement to be done 2.

## Status
Project is: _in progress_

## License
This software is available under the [Apache 2.0 license](./LICENSE), Copyright 2022 [Research group Energy Transition, Windesheim University of Applied Sciences](https://windesheim.nl/energietransitie) 

## Credits
This software is a collaborative effort of:
* Huub Buter · [@Github_handle_1](https://github.com/<github_handle_1>)
* Mirjam van Wee · [@Github_handle_2](https://github.com/<github_handle_2>)
* Kees Fokker · [@Github_handle_3](https://github.com/<github_handle_3>)
 
Product owners:
* Marco Winkelman · [@MarcoW71](https://github.com/MarcoW71)
* Henri ter Hofte · [@henriterhofte](https://github.com/henriterhofte) · Twitter [@HeNRGi](https://twitter.com/HeNRGi)

We use and gratefully acknowlegde the efforts of the makers of the following source code and libraries:
* [OpenTherm Arduino/ESP8266 Library](https://github.com/ihormelnyk/opentherm_library/), by Ihor Melnyk, licensed under [MIT License](https://github.com/ihormelnyk/opentherm_library/blob/master/LICENSE)
* [Twomes OpenTherm monitor](https://github.com/energietransitie/twomes-opentherm-monitor-firmware), by Research Group Energy Transition at Windesheim University of Applied Sciences, licenced under [Apache 2.0](https://raw.githubusercontent.com/energietransitie/twomes-opentherm-monitor-firmware/main/LICENSE)
* [Twomes Boiler Monitor Module Firmware](https://github.com/energietransitie/twomes-boiler-monitor-firmware), by Research Group Energy Transition at Windesheim University of Applied Sciences, licenced under [Apache 2.0](https://raw.githubusercontent.com/energietransitie/twomes-opentherm-monitor-firmware/main/LICENSE)
