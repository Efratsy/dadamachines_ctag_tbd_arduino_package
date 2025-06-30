# dadamachines CTAG TBD Arduino Hardware Package

This repository contains the official Arduino hardware package for the **dadamachines CTAG TBD** platform. It provides the necessary board definitions, core libraries, and tools to program CTAG TBD devices using the Arduino IDE and other compatible environments.

## Core Foundations & Credits

This package integrates and builds upon the incredible work of existing open-source projects. We extend our sincere gratitude to their developers and contributors.

* This package is the official Arduino implementation for the **[CTAG TBD project by ctag-fh-kiel](https://github.com/ctag-fh-kiel/ctag-tbd)**.
* **RP2040 Support**: Our RP2040 implementation is built upon the powerful and feature-rich **[`arduino-pico` core by Earle F. Philhower, III](https.github.com/earlephilhower/arduino-pico)**.
* **ESP32 Support**: Our ESP32 implementation is based on the official **[`arduino-esp32` core by Espressif Systems](https://github.com/espressif/arduino-esp32)**.

## Installation

To install this hardware package via the Arduino IDE, please follow these steps:

1.  Open the Arduino IDE (version 1.8.x or newer).
2.  Go to `File` > `Preferences`.
3.  In the "Additional Board Manager URLs" field, add the following URL:
    ```
    # TO-DO LINK einfügen
    ```
4.  Go to `Tools` > `Board` > `Boards Manager...`.
5.  Search for `dadamachines`.
6.  Click the "Install" button to install the package.
7.  Restart the Arduino IDE.

## Usage

After installation, you can select the target board from the `Tools > Board` menu. The CTAG TBD devices will be listed under the "dadamachines CTAG TBD" category.

## License

This project is the official Arduino implementation for the CTAG TBD platform and is therefore licensed accordingly.

* **Software**: In accordance with its foundation on the CTAG TBD project, this entire Arduino package is licensed under the **GNU General Public License (GPL), Version 3.0**. The full license text is available [here](https://www.gnu.org/licenses/gpl-3.0.html).

* **Hardware**: Any hardware designs related to this project follow the original CTAG TBD hardware license: **Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0)**.

* **Dependencies**: This project incorporates libraries from the `arduino-pico` and `arduino-esp32` cores, which are distributed under the LGPL v2.1. Under the terms of the GPL, the combined work is governed by the GPL 3.0.