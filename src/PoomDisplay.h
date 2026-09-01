#ifndef POOM_DISPLAY_H
#define POOM_DISPLAY_H

/**
 * @file PoomDisplay.h
 * @brief OLED display wrapper for Poom hardware.
 */

#include <Adafruit_SH110X.h>
#include <Wire.h>

#include <stddef.h>
#include <stdint.h>

#include "PoomBoardConfig.h"

/**
 * @brief OLED display wrapper for Poom hardware.
 *
 * Most sketches should use the higher-level ::Poom facade. Use this class when
 * low-level display control or direct SH1106 access is needed.
 */
class PoomDisplay
{
public:
    /**
     * @brief Initialize the I2C bus and OLED display.
     * @return true when initialization succeeds, false otherwise.
     */
    bool begin();

    /** @brief Clear the display driver's internal staging buffer. */
    void clear();

    /** @brief Flush the display driver's internal staging buffer. */
    void show();

    /**
     * @brief Draw one pixel through the display driver.
     * @param x Horizontal position, starting at 0.
     * @param y Vertical position, starting at 0.
     * @param on true for white, false for black.
     */
    void drawPixel(int16_t x, int16_t y, bool on = true);

    /**
     * @brief Set the display driver's text cursor.
     * @param x Cursor X coordinate.
     * @param y Cursor Y coordinate.
     */
    void setCursor(int16_t x, int16_t y);

    /**
     * @brief Set the display driver's text size.
     * @param size Text scale.
     */
    void setTextSize(uint8_t size);

    /** @brief Print a RAM string using the display driver. */
    void print(const char *text);

    /** @brief Print a flash string using the display driver. */
    void print(const __FlashStringHelper *text);

    /** @brief Print a RAM string and newline using the display driver. */
    void println(const char *text);

    /** @brief Print a flash string and newline using the display driver. */
    void println(const __FlashStringHelper *text);

    /**
     * @brief Draw a page-layout 1bpp framebuffer and flush it to the OLED.
     * @param buffer Source buffer using `(y / 8) * width + x` byte layout.
     * @param size Source buffer size in bytes.
     */
    void drawBuffer(const uint8_t *buffer, size_t size = PoomFramebufferSize);

    /** @return Display width in pixels. */
    uint8_t width() const;

    /** @return Display height in pixels. */
    uint8_t height() const;

    /**
     * @brief Access the underlying Adafruit SH1106 object.
     * @return Reference to the display driver.
     */
    Adafruit_SH1106G &native();

private:
    bool initialized_ = false;
    Adafruit_SH1106G oled_{
        PoomScreenWidth,
        PoomScreenHeight,
        &Wire,
        POOM_DISPLAY_RESET_PIN
    };

    bool applyInitSequence();
    void applyTextDefaults();
};

#endif
