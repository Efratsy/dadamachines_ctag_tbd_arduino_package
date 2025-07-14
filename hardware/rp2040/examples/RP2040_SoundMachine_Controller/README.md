# CTAG Dual-Core Sound Machine

A combined RP2040 UI controller and ESP32 audio engine for a versatile hardware synth. The RP2040 (“Controller”) handles all user I/O (buttons, pots, encoder) and drives an OLED + LED UI, sending control data over SPI. The ESP32 (“Audio Engine”) receives that data, runs up to four synth “voices” via a CTAG_Audio library, and streams real-time audio over I²S.

---

## Features

- **Dual-Core, Split Architecture**  
  - **RP2040 (Core 0/1):**  
    - Core 0: Reads 16 buttons, 4 pots, rotary encoder  
    - Core 1: Renders OLED display + 16 LEDs at ~30 FPS  
    - Sends a 5-byte SPI packet (4 × 10 bit pot + 1 × encoder) ~60 Hz  
  - **ESP32 (Core 0/1):**  
    - Core 0: SPI-slave logic task, decodes UI packet  
    - Core 1: Audio task, initializes TLV320 codec + I²S, renders audio in 256-sample blocks  

- **4× Synth Voices** (select via encoder)  
  1. Sine VCO with vibrato LFO  
  2. Square VCO with PWM duty-cycle  
  3. Saw VCO with waveform skew  
  4. 2-operator FM synth (carrier + modulator)

- **4 Potentiometer Assignments** (per-voice)  
  - **Sine:** frequency, amplitude, LFO rate, LFO depth  
  - **Square:** frequency, amplitude, duty cycle, (reserved)  
  - **Saw:**   frequency, amplitude, skew, (reserved)  
  - **FM:**    carrier Hz, amplitude, mod Hz, modulation index  

- **Thread-Safe Mailbox** via FreeRTOS spinlock  
- **Auto-retry Codec Init** on ESP32 cold-boot  

---

## Hardware Requirements

- **RP2040 board** (e.g. dadamachines_rp2040 + CTAG Extension Board)  
  - 16 buttons + LEDs, 4 pots, rotary encoder, I²C OLED  
- **ESP32 board** (e.g. dadamachines_esp32 + TLV320AIC3254 codec)  
  - I²C for codec, I²S pins for audio out  
- SPI bus between RP2040 (master) and ESP32 (slave)  
- Headphones or amplifier on the codec’s line-out/headphone jack  
- MIDI controllers or other peripherals (optional)

---

## Software & Libraries

- **Arduino IDE** (≥ 1.8.x)  
- **Board Packages:**  
  - RP2040: `dadamachines_ctag_tbd` (RP2040 core)  
  - ESP32:  `dadamachines_ctag_tbd` (ESP32 core)  
- **CTAG Libraries:**  
  - `CTAG_ExtensionBoard`, `CTAG_Display`, `CTAG_SPI_IPC` (RP2040)  
  - `CTAG_Audio`, `CTAG_SPI_IPC` (ESP32)  
- **Dependencies:**  
  - FreeRTOS (built into both cores)  
  - Wire.h (I²C), SPI.h (SPI), driver/i2s.h (ESP32 I²S)

---

## Wiring & Setup

1. **RP2040 → ESP32 SPI**  
   - RP2040 MOSI → ESP32 MISO  
   - RP2040 MISO → ESP32 MOSI  
   - RP2040 SCK  → ESP32 SCK  
   - RP2040 SS   → ESP32 CS  

2. **Codec I²C** (ESP32)  
   - SDA → codec SDA  
   - SCL → codec SCL  

3. **OLED I²C** (RP2040)  
   - SDA → display SDA  
   - SCL → display SCL  

4. **Extension Board** (RP2040)  
   - Pots, buttons, encoder wired to CTAG_ExtensionBoard pins  

5. **I²S Audio Out** (ESP32)  
   - I²S MCLK/BCLK/WS/SD → codec / DAC  

---

## Building & Upload

1. **Install** all CTAG and dependency libraries in `/Arduino/libraries/`.  
2. **Open**  
   - `ESP32_SoundMachine_AudioEngine.ino` in Arduino IDE (select your ESP32 board).  
   - `RP2040_SoundMachine_Controller.ino` in a separate window (select your RP2040 board).  
3. **Upload** the RP2040 sketch first, then the ESP32 sketch.  
4. **Power-up** both boards. The RP2040 UI will light, then the ESP32 audio will start after codec init.

---

## Operation

1. **Rotate Encoder** to select between Sine / Square / Saw / FM modes.  
2. **Turn Pots** to adjust the four parameters for the selected voice.  
3. **Listen** on headphones or through your amplifier.  
4. **Re-power** the ESP32 if you ever lose sound; the auto-retry loop ensures the codec always comes up reliably.

---

## License

This project is licensed under the MIT License. See the `LICENSE` file for more details.