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

/** @brief Runtime counters for framebuffer presentation and I2C transfers. */
struct PoomDisplayMetrics
{
    uint32_t presentCount = 0;
    uint32_t transferCount = 0;
    uint32_t skippedCount = 0;
    uint32_t fullTransferCount = 0;
    uint32_t partialTransferCount = 0;
    uint32_t errorCount = 0;
    uint64_t dataBytesSent = 0;
    uint32_t lastPresentMicros = 0;
    uint32_t maxPresentMicros = 0;
    uint16_t lastDataBytes = 0;
    uint16_t presentFps = 0;
    uint16_t transferFps = 0;
};

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
     * @brief Send a page-layout 1bpp framebuffer to the OLED.
     * @param buffer Source buffer using `(y / 8) * width + x` byte layout.
     * @param size Source buffer size in bytes.
     */
    void drawBuffer(const uint8_t *buffer, size_t size = PoomFramebufferSize);

    /** @brief Set the OLED I2C clock without changing the board-wide default. */
    void setI2CClock(uint32_t clockHz);

    /** @return Current OLED I2C clock in hertz. */
    uint32_t i2cClock() const;

    /** @return Current presentation, transfer, byte, timing, and FPS counters. */
    const PoomDisplayMetrics &metrics() const;

    /** @brief Reset display counters without invalidating the OLED shadow. */
    void resetMetrics();

    /** @brief Force the next framebuffer presentation to transmit every byte. */
    void invalidateShadow();

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
    bool shadowValid_ = false;
    bool metricsWindowStarted_ = false;
    uint32_t i2cClockHz_ = POOM_DISPLAY_I2C_CLOCK_HZ;
    size_t i2cDataCapacity_ = 31;
    uint32_t metricsWindowStartMs_ = 0;
    uint16_t windowPresentCount_ = 0;
    uint16_t windowTransferCount_ = 0;
    uint8_t shadowBuffer_[PoomFramebufferSize] = {};
    PoomDisplayMetrics metrics_;
    Adafruit_SH1106G oled_{
        PoomScreenWidth,
        PoomScreenHeight,
        &Wire,
        POOM_DISPLAY_RESET_PIN
    };

    void applyTextDefaults();
    bool drawBufferDirect(const uint8_t *buffer, size_t &dataBytesSent);
    bool sendPageRange(
        uint8_t page,
        uint8_t startColumn,
        const uint8_t *data,
        size_t length,
        size_t &dataBytesSent
    );
    void recordPresentation(size_t dataBytesSent, uint32_t elapsedMicros, bool error);
    void drawBufferWithRotation(const uint8_t *buffer);
};

#endif
