# Poom Board Definition

This folder contains the Poom-specific Arduino board metadata:

- `boards.txt` registers `Poom C5`.
- `variants/poom_c5/pins_arduino.h` defines the board pin map.

The board definition is intentionally kept outside `src/` because it belongs to
the Arduino hardware package layer, not to the Poom library itself.

## Pin Map

| Peripheral | Arduino alias | GPIO |
| --- | --- | --- |
| OLED I2C SDA | `SDA` | 0 |
| OLED I2C SCL | `SCL` | 1 |
| SD SPI MISO | `MISO`, `SD_MISO` | 8 |
| SD SPI MOSI | `MOSI`, `SD_MOSI` | 4 |
| SD SPI SCK | `SCK`, `SD_SCK` | 6 |
| SD SPI CS | `SS`, `SD_CS` | 5 |
| Button A | `PIN_POOM_BUTTON_A` | 28 |
| Button B | `PIN_POOM_BUTTON_B` | 9 |
| Button Left | `PIN_POOM_BUTTON_LEFT` | 3 |
| Button Right | `PIN_POOM_BUTTON_RIGHT` | 23 |
| Button Up | `PIN_POOM_BUTTON_UP` | 7 |
| Button Down | `PIN_POOM_BUTTON_DOWN` | 24 |
| Buzzer | `PIN_POOM_BUZZER` | 26 |
| RGB LEDs | `RGB_BUILTIN`, `PIN_POOM_NEOPIXEL` | 27 |
| NFC interrupt | `PIN_POOM_NFC_INT` | 2 |
| IR TX | `PIN_POOM_IR_TX` | 25 |
| IR RX | `PIN_POOM_IR_RX` | 10 |

## Notes

This is not a copy of the ESP32 Arduino core. A complete Board Manager release
still needs to be packaged against the Espressif Arduino ESP32 core/toolchain.
