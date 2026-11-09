# Twomes boiler BASE firmware

For the associated hardware design files for the Twomes boiler BASE and enclosure and tips and instructions how to produce and assemble the hardware, please see the [twomes-boiler-base-hardware](https://github.com/energiegedrag-nl/needforheat-boiler-monitor-hardware) repository. 

> **Own deployment.** This repository is maintained by [energiegedrag-nl](https://github.com/energiegedrag-nl) and deployed under `energiegedrag.nl`. To run your own instance, replace `energiegedrag-nl` by your GitHub organisation and `energiegedrag.nl` by your domain (hosts used: `api.<domain>`, `manuals.<domain>`), consistently in configuration and build settings.

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
The Twomes Presence Detector primarily serves as an example and template repository how to create firmware that uses the [twomes-generic-esp-firmware](https://github.com/energiegedrag-nl/needforheat-generic-firmware) library. Nevertheless, it can be used in  measurement campaigns that require occupancy detection using Bluetooth name requests.

## Measurements

A Twomes boiler BASE device, in addition to [generic data sent by any Twomes measurement device](https://github.com/energiegedrag-nl/needforheat-generic-firmware#readme), sends data about the following properties via the [Twomes API](https://github.com/energiegedrag-nl/needforheat-server-api) to a Twomes server:

| Sensor  | Property                 | Unit                   | Printf format | Measurement interval [h:mm:ss] | OpenTherm ID | Part | Description                                |
|---------|--------------------------|------------------------|---------------|--------------------------------|--------------|------|--------------------------------------------|
|         | flame__bool          | bool (0=False; 1=True) | %d            | 0:01:00                        | 0            | LB/3 | STATUS /Flame status                       |
|         | ch__bool   | bool (0=False; 1=True) | %d            | 0:01:00                        | 0            | LB/1 | STATUS/CH mode                             |
|         | dhw__bool | bool (0=False; 1=True) | %d            | 0:01:00                        | 0            | LB/2 | STATUS/DHW mode                            |
|         | mod_max__0       | %                      | %.2f            | 0:01:00                        | 14           |      | CAPACITY SETTING                           |
|         | cap_max__kW             | kW                     | %d            | 0:01:00                        | 15           | HB   | MAX CAPACITY                               |
|         | mod_min__0       | %                      | %.2f            | 0:01:00                        | 15           | LB   | MIN-MOD-LEVEL                              |
|         | temp_set__degC         | °C                     | %.2f          | 0:05:00                        | 16           |      | ROOM SETPOINT                              |
|         | mod__0  | %                      | %.2f            | 0:01:00                        | 17           |      | RELATIVE MODULATION LEVEL                  |
|         | temp_in__degC                 | °C                     | %.2f          | 0:05:00                        | 24           |      | ROOM TEMPERATURE                           |
|         | temp_ch_max__degC      | °C                     | %.2f          | 0:05:00                        | 57           |      | MAX CH WATER SETPOINT                      |
|         | temp_sup__degC         | °C                     | %.2f          | 0:01:00                        | 25           |      | BOILER WATER TEMP.                         |
|         | temp_ret__degC         | °C                     | %.2f          | 0:01:00                        | 28           |      | RETURN WATER TEMPERATURE                   |
| DS18b20 | temp1__degC              | °C                     | %.2f          | 0:01:00                        |              |      | temperature of hydronic supply/return pipe |
| DS18b20 | temp2__degC              | °C                     | %.2f          | 0:01:00                        |              |      | temperature of hydronic supply/return pipe |

## Deploying
To deploy this software, see the [deploying section in the twomes-generic-esp-firmware library documentation](https://github.com/energiegedrag-nl/needforheat-generic-firmware/blob/main/docs/deploying/prerequisites.md). The firmware needed can be found as a [release from this repository](https://github.com/energiegedrag-nl/needforheat-presence-detector-firmware/releases).

## Developing
To develop software for, or based on this software, see the [developing section in the twomes-generic-esp-firmware library documentation](https://github.com/energiegedrag-nl/needforheat-generic-firmware/blob/main/docs/starting/prerequisites.md)

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
* [Twomes OpenTherm monitor](https://github.com/energiegedrag-nl/twomes-opentherm-monitor-firmware), by Research Group Energy Transition at Windesheim University of Applied Sciences, licenced under [Apache 2.0](https://raw.githubusercontent.com/energietransitie/twomes-opentherm-monitor-firmware/main/LICENSE)
* [Twomes Boiler Monitor Module Firmware](https://github.com/energietransitie/twomes-boiler-monitor-firmware), by Research Group Energy Transition at Windesheim University of Applied Sciences, licenced under [Apache 2.0](https://raw.githubusercontent.com/energietransitie/twomes-opentherm-monitor-firmware/main/LICENSE)
