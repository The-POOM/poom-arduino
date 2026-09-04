#ifndef POOM_BOARD_CONFIG_H
#define POOM_BOARD_CONFIG_H

/**
 * @file PoomBoardConfig.h
 * @brief Compile-time board configuration for Poom hardware.
 */

#include <Arduino.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/** @def POOM_SCREEN_WIDTH
 *  @brief OLED width in pixels.
 */
#ifndef POOM_SCREEN_WIDTH
#define POOM_SCREEN_WIDTH 128
#endif

/** @def POOM_SCREEN_HEIGHT
 *  @brief OLED height in pixels.
 */
#ifndef POOM_SCREEN_HEIGHT
#define POOM_SCREEN_HEIGHT 64
#endif

/** @def POOM_I2C_SDA_PIN
 *  @brief I2C SDA pin used by the OLED.
 */
#ifndef POOM_I2C_SDA_PIN
#define POOM_I2C_SDA_PIN 0
#endif

/** @def POOM_I2C_SCL_PIN
 *  @brief I2C SCL pin used by the OLED.
 */
#ifndef POOM_I2C_SCL_PIN
#define POOM_I2C_SCL_PIN 1
#endif

/** @def POOM_BUTTON_A_PIN
 *  @brief GPIO for the A button.
 */
#ifndef POOM_BUTTON_A_PIN
#define POOM_BUTTON_A_PIN 28
#endif

/** @def POOM_BUTTON_B_PIN
 *  @brief GPIO for the B button.
 */
#ifndef POOM_BUTTON_B_PIN
#define POOM_BUTTON_B_PIN 9
#endif

/** @def POOM_BUTTON_LEFT_PIN
 *  @brief GPIO for the Left button.
 */
#ifndef POOM_BUTTON_LEFT_PIN
#define POOM_BUTTON_LEFT_PIN 3
#endif

/** @def POOM_BUTTON_RIGHT_PIN
 *  @brief GPIO for the Right button.
 */
#ifndef POOM_BUTTON_RIGHT_PIN
#define POOM_BUTTON_RIGHT_PIN 23
#endif

/** @def POOM_BUTTON_UP_PIN
 *  @brief GPIO for the Up button.
 */
#ifndef POOM_BUTTON_UP_PIN
#define POOM_BUTTON_UP_PIN 7
#endif

/** @def POOM_BUTTON_DOWN_PIN
 *  @brief GPIO for the Down button.
 */
#ifndef POOM_BUTTON_DOWN_PIN
#define POOM_BUTTON_DOWN_PIN 24
#endif

/** @def POOM_BUZZER_PIN
 *  @brief GPIO used by the buzzer.
 */
#ifndef POOM_BUZZER_PIN
#define POOM_BUZZER_PIN 26
#endif

/** @def POOM_NEOPIXEL_PIN
 *  @brief GPIO used by the onboard LED strip.
 */
#ifndef POOM_NEOPIXEL_PIN
#define POOM_NEOPIXEL_PIN 27
#endif

/** @def POOM_LED_COUNT
 *  @brief Number of onboard LEDs.
 */
#ifndef POOM_LED_COUNT
#define POOM_LED_COUNT 9
#endif

/** @def POOM_DISPLAY_I2C_ADDR
 *  @brief OLED I2C address.
 */
#ifndef POOM_DISPLAY_I2C_ADDR
#define POOM_DISPLAY_I2C_ADDR 0x3C
#endif

/** @def POOM_DISPLAY_I2C_CLOCK_HZ
 *  @brief Default OLED I2C clock. The IMU selects its own 400 kHz clock.
 */
#ifndef POOM_DISPLAY_I2C_CLOCK_HZ
#define POOM_DISPLAY_I2C_CLOCK_HZ 800000UL
#endif

/** @def POOM_DISPLAY_RESET_PIN
 *  @brief OLED reset pin, or -1 when no reset pin is used.
 */
#ifndef POOM_DISPLAY_RESET_PIN
#define POOM_DISPLAY_RESET_PIN -1
#endif

/** @def POOM_DISPLAY_ROTATION
 *  @brief OLED rotation value passed to the display driver.
 */
#ifndef POOM_DISPLAY_ROTATION
#define POOM_DISPLAY_ROTATION 0
#endif

/** @def POOM_DISPLAY_X_SHIFT
 *  @brief Horizontal circular shift applied while flushing the framebuffer.
 */
#ifndef POOM_DISPLAY_X_SHIFT
#define POOM_DISPLAY_X_SHIFT 0
#endif

/** @def POOM_IMU_DEFAULT_ADDRESS
 *  @brief Default LSM6DS3TR-C I2C address when SA0 is high.
 */
#ifndef POOM_IMU_DEFAULT_ADDRESS
#define POOM_IMU_DEFAULT_ADDRESS 0x6B
#endif

/** @def POOM_IMU_ALTERNATE_ADDRESS
 *  @brief Alternate LSM6DS3TR-C I2C address when SA0 is low.
 */
#ifndef POOM_IMU_ALTERNATE_ADDRESS
#define POOM_IMU_ALTERNATE_ADDRESS 0x6A
#endif

/** @def POOM_IMU_I2C_CLOCK_HZ
 *  @brief I2C clock used for LSM6DS3TR-C transactions.
 */
#ifndef POOM_IMU_I2C_CLOCK_HZ
#define POOM_IMU_I2C_CLOCK_HZ 400000UL
#endif

/** @def POOM_USE_NEOPIXEL
 *  @brief Feature flag for onboard NeoPixel support.
 */
#ifndef POOM_USE_NEOPIXEL
#define POOM_USE_NEOPIXEL 1
#endif

/** @brief Compile-time screen width in pixels. */
static constexpr uint8_t PoomScreenWidth = POOM_SCREEN_WIDTH;

/** @brief Compile-time screen height in pixels. */
static constexpr uint8_t PoomScreenHeight = POOM_SCREEN_HEIGHT;

/** @brief Number of bytes in the 1bpp display framebuffer. */
static constexpr size_t PoomFramebufferSize =
    (static_cast<size_t>(PoomScreenWidth) * static_cast<size_t>(PoomScreenHeight)) / 8U;

#ifndef PROGMEM
#define PROGMEM
#endif

#ifndef pgm_read_byte
#define pgm_read_byte(addr) (*(const uint8_t *)(addr))
#endif

#ifndef pgm_read_word
#define pgm_read_word(addr) (*(const uint16_t *)(addr))
#endif

#ifndef pgm_read_ptr
#define pgm_read_ptr(addr) (*(const void *const *)(addr))
#endif

#ifndef PSTR
#define PSTR(text) (text)
#endif

#ifndef strlen_P
#define strlen_P(value) strlen(value)
#endif

#endif
