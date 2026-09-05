#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <stdint.h>
#include "soc/soc_caps.h"

// USB serial pins are handled by the ESP32-C5 USB peripheral when CDC is used.
static const uint8_t TX = 11;
static const uint8_t RX = 12;

// OLED and shared I2C bus.
static const uint8_t SDA = 0;
static const uint8_t SCL = 1;
#define I2C_SDA_PIN SDA
#define I2C_SCL_PIN SCL

// SD card SPI bus.
static const uint8_t MISO = 8;
static const uint8_t MOSI = 4;
static const uint8_t SCK = 6;
static const uint8_t SS = 5;
#define PIN_NUM_MISO MISO
#define PIN_NUM_MOSI MOSI
#define PIN_NUM_CLK SCK
#define PIN_NUM_CS SS

// Poom controls and peripherals.
static const uint8_t PIN_POOM_BUTTON_A = 28;
static const uint8_t PIN_POOM_BUTTON_B = 9;
static const uint8_t PIN_POOM_BUTTON_LEFT = 3;
static const uint8_t PIN_POOM_BUTTON_RIGHT = 23;
static const uint8_t PIN_POOM_BUTTON_UP = 7;
static const uint8_t PIN_POOM_BUTTON_DOWN = 24;
static const uint8_t PIN_POOM_BUZZER = 26;
static const uint8_t PIN_POOM_NEOPIXEL = 27;
static const uint8_t PIN_POOM_NFC_INT = 2;
static const uint8_t PIN_POOM_IR_TX = 25;
static const uint8_t PIN_POOM_IR_RX = 10;
static const uint8_t POOM_NEOPIXEL_COUNT = 9;
#define PIN_NUM_A PIN_POOM_BUTTON_A
#define PIN_NUM_B PIN_POOM_BUTTON_B
#define PIN_NUM_LEFT PIN_POOM_BUTTON_LEFT
#define PIN_NUM_RIGHT PIN_POOM_BUTTON_RIGHT
#define PIN_NUM_UP PIN_POOM_BUTTON_UP
#define PIN_NUM_DOWN PIN_POOM_BUTTON_DOWN
#define PIN_NUM_BUZZER PIN_POOM_BUZZER
#define PIN_NUM_WS2812 PIN_POOM_NEOPIXEL
#define PIN_NUM_INT_NFC PIN_POOM_NFC_INT
#define PIN_NUM_IR_TX PIN_POOM_IR_TX
#define PIN_NUM_IR_RX PIN_POOM_IR_RX

// Arduino-compatible aliases.
static const uint8_t LED_BUILTIN = PIN_POOM_NEOPIXEL;
static const uint8_t BUTTON_BUILTIN = PIN_POOM_BUTTON_A;
#define BUILTIN_LED LED_BUILTIN
#define RGB_BUILTIN LED_BUILTIN
#define RGB_BRIGHTNESS 64

// SD-friendly aliases used by common ESP32 examples.
static const uint8_t SD_MISO = MISO;
static const uint8_t SD_MOSI = MOSI;
static const uint8_t SD_SCK = SCK;
static const uint8_t SD_CS = SS;

// Poom library aliases.
#ifndef POOM_I2C_SDA_PIN
#define POOM_I2C_SDA_PIN SDA
#endif

#ifndef POOM_I2C_SCL_PIN
#define POOM_I2C_SCL_PIN SCL
#endif

#ifndef POOM_SD_MISO_PIN
#define POOM_SD_MISO_PIN MISO
#endif

#ifndef POOM_SD_MOSI_PIN
#define POOM_SD_MOSI_PIN MOSI
#endif

#ifndef POOM_SD_SCK_PIN
#define POOM_SD_SCK_PIN SCK
#endif

#ifndef POOM_SD_CS_PIN
#define POOM_SD_CS_PIN SS
#endif

#ifndef POOM_BUTTON_A_PIN
#define POOM_BUTTON_A_PIN PIN_POOM_BUTTON_A
#endif

#ifndef POOM_BUTTON_B_PIN
#define POOM_BUTTON_B_PIN PIN_POOM_BUTTON_B
#endif

#ifndef POOM_BUTTON_LEFT_PIN
#define POOM_BUTTON_LEFT_PIN PIN_POOM_BUTTON_LEFT
#endif

#ifndef POOM_BUTTON_RIGHT_PIN
#define POOM_BUTTON_RIGHT_PIN PIN_POOM_BUTTON_RIGHT
#endif

#ifndef POOM_BUTTON_UP_PIN
#define POOM_BUTTON_UP_PIN PIN_POOM_BUTTON_UP
#endif

#ifndef POOM_BUTTON_DOWN_PIN
#define POOM_BUTTON_DOWN_PIN PIN_POOM_BUTTON_DOWN
#endif

#ifndef POOM_BUZZER_PIN
#define POOM_BUZZER_PIN PIN_POOM_BUZZER
#endif

#ifndef POOM_NEOPIXEL_PIN
#define POOM_NEOPIXEL_PIN PIN_POOM_NEOPIXEL
#endif

#ifndef POOM_LED_COUNT
#define POOM_LED_COUNT POOM_NEOPIXEL_COUNT
#endif

#ifndef POOM_NFC_INT_PIN
#define POOM_NFC_INT_PIN PIN_POOM_NFC_INT
#endif

#ifndef POOM_IR_TX_PIN
#define POOM_IR_TX_PIN PIN_POOM_IR_TX
#endif

#ifndef POOM_IR_RX_PIN
#define POOM_IR_RX_PIN PIN_POOM_IR_RX
#endif

// LP I2C pins are fixed by the ESP32-C5 low-power peripheral.
static const uint8_t LP_SDA = 2;
static const uint8_t LP_SCL = 3;
#define WIRE1_PIN_DEFINED
#define SDA1 LP_SDA
#define SCL1 LP_SCL

// LP UART pins are fixed by the ESP32-C5 low-power peripheral.
static const uint8_t LP_RX = 4;
static const uint8_t LP_TX = 5;

#endif
