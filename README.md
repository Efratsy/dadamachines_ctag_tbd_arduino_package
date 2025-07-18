# dadamachines CTAG TBD – Arduino Hardware Package & Libraries

This repository contains the Arduino **hardware package** and **core libraries** for the [CTAG TBD platform](https://github.com/ctag-fh-kiel/ctag-tbd).

It enables easy development for CTAG TBD devices using the Arduino IDE or other compatible environments.

---

## Hardware Support

This package provides official board definitions, tools, and build support for the following platforms:

- **RP2040**: Based on [`arduino-pico`](https://github.com/earlephilhower/arduino-pico) by Earle F. Philhower, III  
- **ESP32**: Based on the official [`arduino-esp32`](https://github.com/espressif/arduino-esp32) core by Espressif Systems

All components are integrated under the unified `dadamachines CTAG TBD` platform.

---

## Installation via Arduino IDE

1. Open the Arduino IDE (version 1.8.x or newer).
2. Go to `File` > `Preferences`.
3. In the **"Additional Board Manager URLs"** field, add:
    ```
    https://efratsy.github.io/dadamachines_ctag_tbd_arduino_package/package_dadamachines_ctag_tbd_index.json
    ```
4. Go to `Tools` > `Board` > `Boards Manager...`.
5. Search for **dadamachines**.
6. Click **Install**.
7. After installation, select your board via `Tools > Board > dadamachines CTAG TBD`.

---

## Included Libraries

These official libraries are bundled with the hardware package and tightly integrated with CTAG TBD devices:

| Library               | Description                                                                 |
|-----------------------|-----------------------------------------------------------------------------|
| `CTAG_Audio`          | High-level API for audio I/O, codecs, and DSP control                       |
| `CTAG_Button`         | Easy-to-use button debouncing and input reading                             |
| `CTAG_Display`        | OLED display support, including text, shapes, and UI rendering              |
| `CTAG_ExtensionBoard` | Communication with optional extension boards                                |
| `CTAG_LED`            | LED control including brightness and animation patterns                     |
| `CTAG_TRSMIDI`        | Handling of TRS-MIDI input and output via dedicated hardware UART           |
| `CTAG_USBMIDI`        | USB MIDI host and device support, including real-time MIDI event handling   |
| `CTAG_SPI_IPC`        | Inter-process communication via SPI between multiple MCUs                   |


These libraries are automatically available when using this hardware package in the Arduino IDE.

---

## Manual Library Use (Advanced)

If needed for development purposes, you can clone this repository and copy the `libraries/` folder manually into your Arduino sketchbook’s `libraries/` directory.

---

## Acknowledgements

This package is built upon the incredible work of the following open-source projects:

- [CTAG TBD hardware and firmware](https://github.com/ctag-fh-kiel/ctag-tbd)
- [arduino-pico](https://github.com/earlephilhower/arduino-pico) by Earle F. Philhower, III
- [arduino-esp32](https://github.com/espressif/arduino-esp32) by Espressif Systems

We thank all contributors to these projects.

---

##  Documentation & Resources

For more detailed usage instructions, example projects, and developer guides, please refer to the official documentation:

- **Wiki** – Getting started, hardware guides, example walkthroughs:  
  [https://github.com/efratsy/dadamachines_ctag_tbd_arduino_package/wiki](https://github.com/efratsy/dadamachines_ctag_tbd_arduino_package/wiki)

- **API Reference** – Full documentation of all bundled libraries (Doxygen):  
  [https://efratsy.github.io/dadamachines_ctag_tbd_arduino_package/api](https://efratsy.github.io/dadamachines_ctag_tbd_arduino_package/api/index.html)

If you're new to the platform, we recommend starting with the [Getting Started](https://github.com/efratsy/dadamachines_ctag_tbd_arduino_package/wiki/Getting-Started) guide in the Wiki.

---

## License

**Software:**  
This Arduino package is licensed under the **GNU General Public License v3.0 (GPLv3)**.  
See: [https://www.gnu.org/licenses/gpl-3.0.html](https://www.gnu.org/licenses/gpl-3.0.html)

**Hardware:**  
All hardware design files related to CTAG TBD are licensed under  
**Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0)**.

**Dependencies:**  
Bundled libraries from `arduino-pico` and `arduino-esp32` fall under LGPL v2.1 and are compatible with the GPLv3 license of this package.

---