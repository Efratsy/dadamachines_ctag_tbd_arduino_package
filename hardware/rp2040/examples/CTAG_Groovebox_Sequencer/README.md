# CTAG RP2040 Groovebox / Step-Sequencer

This is a fully functional, RP2040-based 16-step MIDI sequencer. The project utilizes a dual-core architecture to separate the responsive user interface from the time-critical MIDI logic.

The RP2040 reads inputs from a custom extension board, generates MIDI data, and outputs it simultaneously via USB Host and TRS MIDI. An OLED display and 16 LEDs provide real-time visual feedback.

## Features

* **16-Step Sequencer:** Activate and deactivate each step via the 16 hardware buttons.
* **Real-time Parameter Control:** Adjust tempo, note pitch, velocity, and gate length live using four potentiometers.
* **Visual Feedback:** The OLED display shows all current parameters. The 16 LEDs visualize the programmed pattern, and a "running light" indicates the currently playing step.
* **Dual MIDI Output:** MIDI notes are sent simultaneously to the USB-A (Host) port and the TRS MIDI output.
* **Performant Dual-Core Architecture:** Core 0 manages all musical logic and input processing, while Core 1 is exclusively dedicated to smooth rendering on the display and LEDs.

## Hardware Requirements

* CTAG TBD (e.g., the `dadamachines_rp2040`).
* The "CTAG Extension Board" with 16 buttons, 4 potentiometers, and an encoder.
* An I2C OLED Display (128x64, SH1106).
* A USB MIDI keyboard or synthesizer to connect to the USB-A Host port.
* A synthesizer or audio interface to connect to the TRS MIDI port.

## Software & Libraries

1.  **Arduino IDE**
2.  **RP2040 Board Package:** (e.g., `dadamachines_ctag_tbd`)
3.  **Custom CTAG Libraries:**
    * `CTAG_ExtensionBoard`
    * `CTAG_Display`
    * `CTAG_TRSMIDI`
    * `CTAG_USBMIDI`
4.  **External Libraries:**
    * `Adafruit TinyUSB Library`
    * `usb_midi_host` (the `rppicomidi` driver, installed as a library)

## Installation & Setup

1.  Ensure all the libraries listed above are installed in your `Arduino/libraries` folder.
2.  Open the `CTAG_Groovebox_Sequencer.ino` sketch in the Arduino IDE.
3.  Select your RP2040 board in the `Tools` > `Board` menu.
4.  **IMPORTANT:** Go to `Tools` > `USB Stack` and select the **`Adafruit TinyUSB Host`** option.
5.  Connect all hardware components.
6.  Upload the sketch to the RP2040.

## How it Works

### Architecture

The system utilizes both cores of the RP2040 to ensure a clean separation of tasks:

* **Core 0 (The Logic Core):** Executes `setup()` and `loop()`. It is responsible for:
    * Reading all inputs from the extension board (`extBoard.update()`).
    * All sequencer logic (timing, note triggering).
    * Sending MIDI data via USB and TRS.
    * Updating the "mailbox" variables for Core 1.

* **Core 1 (The UI Core):** Executes `setup1()` and `loop1()`. It is responsible for:
    * Initializing and driving the display and LEDs.
    * Continuously reading the "mailbox" variables and rendering this information.
    * This core does not execute any logic itself; it is a pure "renderer."

### Controls

* **D1 - D16 Buttons:** Toggle the corresponding step in the 16-step pattern on or off. An active note is indicated by a blue LED.
* **Running Light LED:** A bright green LED indicates the currently playing step.
* **Potentiometer 1:** Controls the **Tempo** of the sequencer (40 - 240 BPM).
* **Potentiometer 2:** Sets the **Pitch** of the note played by the sequencer.
* **Potentiometer 3:** Determines the **Velocity** of the note (1 - 127).
* **Potentiometer 4:** Adjusts the **Gate Length** (duration) of the note in milliseconds.

## License

This project is licensed under the MIT License. See the `LICENSE` file for more details.