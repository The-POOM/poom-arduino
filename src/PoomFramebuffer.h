#ifndef POOM_FRAMEBUFFER_H
#define POOM_FRAMEBUFFER_H

/**
 * @file PoomFramebuffer.h
 * @brief Monochrome framebuffer used by Poom graphics and games.
 */

#include <stddef.h>
#include <stdint.h>

#include "PoomBoardConfig.h"

/**
 * @brief 128x64 monochrome framebuffer used by Poom rendering.
 *
 * The buffer uses page layout: each byte stores a vertical column of 8 pixels,
 * and rows are grouped into pages. The byte index is `(y / 8) * width + x`.
 */
class PoomFramebuffer
{
public:
    /**
     * @brief Get writable raw framebuffer data.
     * @return Pointer to PoomFramebufferSize bytes.
     */
    uint8_t *data();

    /**
     * @brief Get read-only raw framebuffer data.
     * @return Pointer to PoomFramebufferSize bytes.
     */
    const uint8_t *data() const;

    /**
     * @brief Get the framebuffer size in bytes.
     * @return Number of bytes in the framebuffer.
     */
    size_t size() const;

    /** @brief Clear the framebuffer to black. */
    void clear();

    /**
     * @brief Fill the framebuffer.
     * @param on true for white, false for black.
     */
    void fill(bool on);

    /** @brief Invert all pixels in the framebuffer. */
    void invert();

    /**
     * @brief Draw one pixel.
     * @param x Horizontal position, starting at 0.
     * @param y Vertical position, starting at 0.
     * @param on true to set the pixel, false to clear it.
     */
    void drawPixel(int16_t x, int16_t y, bool on = true);

    /**
     * @brief Draw a clipped horizontal line.
     * @param x Left edge of the line.
     * @param y Vertical position of the line.
     * @param width Line width in pixels. Non-positive values draw nothing.
     * @param on true to set pixels, false to clear them.
     */
    void drawHorizontalLine(int16_t x, int16_t y, int16_t width, bool on = true);

    /**
     * @brief Draw a clipped vertical line.
     * @param x Horizontal position of the line.
     * @param y Top edge of the line.
     * @param height Line height in pixels. Non-positive values draw nothing.
     * @param on true to set pixels, false to clear them.
     */
    void drawVerticalLine(int16_t x, int16_t y, int16_t height, bool on = true);

    /** @brief Draw a clipped line between two endpoints. */
    void drawLine(
        int16_t x0,
        int16_t y0,
        int16_t x1,
        int16_t y1,
        bool on = true
    );

    /** @brief Draw a clipped circle outline. */
    void drawCircle(int16_t x, int16_t y, int16_t radius, bool on = true);

    /** @brief Draw a clipped filled circle. */
    void fillCircle(int16_t x, int16_t y, int16_t radius, bool on = true);

    /**
     * @brief Draw a clipped filled rectangle.
     * @param x Left edge of the rectangle.
     * @param y Top edge of the rectangle.
     * @param width Rectangle width in pixels. Non-positive values draw nothing.
     * @param height Rectangle height in pixels. Non-positive values draw nothing.
     * @param on true to set pixels, false to clear them.
     */
    void fillRect(int16_t x, int16_t y, int16_t width, int16_t height, bool on = true);

    /**
     * @brief Draw a clipped rectangle outline.
     * @param x Left edge of the rectangle.
     * @param y Top edge of the rectangle.
     * @param width Rectangle width in pixels. Non-positive values draw nothing.
     * @param height Rectangle height in pixels. Non-positive values draw nothing.
     * @param on true to set pixels, false to clear them.
     */
    void drawRect(int16_t x, int16_t y, int16_t width, int16_t height, bool on = true);

    /** @brief Draw a clipped rounded rectangle outline. */
    void drawRoundRect(
        int16_t x,
        int16_t y,
        int16_t width,
        int16_t height,
        int16_t radius,
        bool on = true
    );

    /** @brief Draw a clipped filled rounded rectangle. */
    void fillRoundRect(
        int16_t x,
        int16_t y,
        int16_t width,
        int16_t height,
        int16_t radius,
        bool on = true
    );

    /** @brief Draw a clipped triangle outline. */
    void drawTriangle(
        int16_t x0,
        int16_t y0,
        int16_t x1,
        int16_t y1,
        int16_t x2,
        int16_t y2,
        bool on = true
    );

    /** @brief Draw a clipped filled triangle. */
    void fillTriangle(
        int16_t x0,
        int16_t y0,
        int16_t x1,
        int16_t y1,
        int16_t x2,
        int16_t y2,
        bool on = true
    );

    /**
     * @brief Read one pixel.
     * @param x Horizontal position, starting at 0.
     * @param y Vertical position, starting at 0.
     * @return true when the pixel is set, false when clear or outside bounds.
     */
    bool getPixel(int16_t x, int16_t y) const;

    /**
     * @brief Set the text cursor.
     * @param x Cursor X coordinate in pixels.
     * @param y Cursor Y coordinate in pixels.
     */
    void setCursor(int16_t x, int16_t y);

    /**
     * @brief Set the built-in text scale.
     * @param size Pixel scale. Values less than 1 are treated as 1.
     */
    void setTextSize(uint8_t size);

    /** @brief Print a RAM string at the current cursor. */
    void print(const char *text);

    /** @brief Print a flash string at the current cursor. */
    void print(const __FlashStringHelper *text);

    /** @brief Print a RAM string and advance to the next line. */
    void println(const char *text);

    /** @brief Print a flash string and advance to the next line. */
    void println(const __FlashStringHelper *text);

private:
    uint8_t pixels_[PoomFramebufferSize] = {0};
    int16_t cursorX_ = 0;
    int16_t cursorY_ = 0;
    uint8_t textSize_ = 1;

    bool contains(int16_t x, int16_t y) const;
    size_t indexFor(int16_t x, int16_t y) const;
    uint8_t bitFor(int16_t y) const;
    void drawCircleHelper(
        int16_t x,
        int16_t y,
        int16_t radius,
        uint8_t corners,
        bool on
    );
    void fillCircleHelper(
        int16_t x,
        int16_t y,
        int16_t radius,
        uint8_t sides,
        int16_t delta,
        bool on
    );
    void drawChar(char value);
    void drawGlyphColumn(int16_t x, int16_t y, uint8_t bits, uint8_t scale);
    uint8_t glyphColumn(char value, uint8_t column) const;
};

#endif
