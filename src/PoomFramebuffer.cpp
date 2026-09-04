#include "PoomFramebuffer.h"

#include <string.h>

#include <Arduino.h>

uint8_t *PoomFramebuffer::data()
{
    return pixels_;
}

const uint8_t *PoomFramebuffer::data() const
{
    return pixels_;
}

size_t PoomFramebuffer::size() const
{
    return PoomFramebufferSize;
}

void PoomFramebuffer::clear()
{
    fill(false);
}

void PoomFramebuffer::fill(bool on)
{
    memset(pixels_, on ? 0xFF : 0x00, sizeof(pixels_));
}

void PoomFramebuffer::invert()
{
    for (size_t i = 0; i < sizeof(pixels_); ++i) {
        pixels_[i] = static_cast<uint8_t>(~pixels_[i]);
    }
}

void PoomFramebuffer::drawPixel(int16_t x, int16_t y, bool on)
{
    if (!contains(x, y)) {
        return;
    }

    const size_t index = indexFor(x, y);
    const uint8_t bit = bitFor(y);

    if (on) {
        pixels_[index] |= bit;
    } else {
        pixels_[index] &= static_cast<uint8_t>(~bit);
    }
}

void PoomFramebuffer::drawHorizontalLine(int16_t x, int16_t y, int16_t width, bool on)
{
    if (width <= 0 || y < 0 || y >= PoomScreenHeight) {
        return;
    }

    int32_t startX = x;
    int32_t endX = startX + width;

    if (endX <= 0 || startX >= PoomScreenWidth) {
        return;
    }

    if (startX < 0) {
        startX = 0;
    }
    if (endX > PoomScreenWidth) {
        endX = PoomScreenWidth;
    }

    size_t index = indexFor(static_cast<int16_t>(startX), y);
    const uint8_t bit = bitFor(y);
    const uint8_t clearMask = static_cast<uint8_t>(~bit);

    for (int32_t drawX = startX; drawX < endX; ++drawX, ++index) {
        if (on) {
            pixels_[index] |= bit;
        } else {
            pixels_[index] &= clearMask;
        }
    }
}

void PoomFramebuffer::drawVerticalLine(int16_t x, int16_t y, int16_t height, bool on)
{
    if (height <= 0 || x < 0 || x >= PoomScreenWidth) {
        return;
    }

    int32_t startY = y;
    int32_t endY = startY + height;

    if (endY <= 0 || startY >= PoomScreenHeight) {
        return;
    }

    if (startY < 0) {
        startY = 0;
    }
    if (endY > PoomScreenHeight) {
        endY = PoomScreenHeight;
    }

    for (int32_t drawY = startY; drawY < endY; ++drawY) {
        drawPixel(x, static_cast<int16_t>(drawY), on);
    }
}

void PoomFramebuffer::fillRect(int16_t x, int16_t y, int16_t width, int16_t height, bool on)
{
    if (width <= 0 || height <= 0) {
        return;
    }

    int32_t startX = x;
    int32_t startY = y;
    int32_t endX = startX + width;
    int32_t endY = startY + height;

    if (endX <= 0 || endY <= 0 || startX >= PoomScreenWidth || startY >= PoomScreenHeight) {
        return;
    }

    if (startX < 0) {
        startX = 0;
    }
    if (startY < 0) {
        startY = 0;
    }
    if (endX > PoomScreenWidth) {
        endX = PoomScreenWidth;
    }
    if (endY > PoomScreenHeight) {
        endY = PoomScreenHeight;
    }

    const int16_t clippedX = static_cast<int16_t>(startX);
    const int16_t clippedWidth = static_cast<int16_t>(endX - startX);
    for (int32_t drawY = startY; drawY < endY; ++drawY) {
        drawHorizontalLine(clippedX, static_cast<int16_t>(drawY), clippedWidth, on);
    }
}

void PoomFramebuffer::drawRect(int16_t x, int16_t y, int16_t width, int16_t height, bool on)
{
    if (width <= 0 || height <= 0) {
        return;
    }

    const int32_t right = static_cast<int32_t>(x) + width - 1;
    const int32_t bottom = static_cast<int32_t>(y) + height - 1;

    if (y >= 0 && y < PoomScreenHeight) {
        drawHorizontalLine(x, y, width, on);
    }
    if (bottom >= 0 && bottom < PoomScreenHeight) {
        drawHorizontalLine(x, static_cast<int16_t>(bottom), width, on);
    }
    if (x >= 0 && x < PoomScreenWidth) {
        drawVerticalLine(x, y, height, on);
    }
    if (right >= 0 && right < PoomScreenWidth) {
        drawVerticalLine(static_cast<int16_t>(right), y, height, on);
    }
}

bool PoomFramebuffer::getPixel(int16_t x, int16_t y) const
{
    if (!contains(x, y)) {
        return false;
    }

    return (pixels_[indexFor(x, y)] & bitFor(y)) != 0;
}

void PoomFramebuffer::setCursor(int16_t x, int16_t y)
{
    cursorX_ = x;
    cursorY_ = y;
}

void PoomFramebuffer::setTextSize(uint8_t size)
{
    textSize_ = size == 0 ? 1 : size;
}

void PoomFramebuffer::print(const char *text)
{
    if (!text) {
        return;
    }

    while (*text) {
        drawChar(*text++);
    }
}

void PoomFramebuffer::print(const __FlashStringHelper *text)
{
    print(reinterpret_cast<const char *>(text));
}

void PoomFramebuffer::println(const char *text)
{
    print(text);
    drawChar('\n');
}

void PoomFramebuffer::println(const __FlashStringHelper *text)
{
    print(text);
    drawChar('\n');
}

bool PoomFramebuffer::contains(int16_t x, int16_t y) const
{
    return x >= 0 && y >= 0 && x < PoomScreenWidth && y < PoomScreenHeight;
}

size_t PoomFramebuffer::indexFor(int16_t x, int16_t y) const
{
    const size_t page = static_cast<size_t>(y) >> 3;
    return (page * PoomScreenWidth) + static_cast<size_t>(x);
}

uint8_t PoomFramebuffer::bitFor(int16_t y) const
{
    return static_cast<uint8_t>(1U << (y & 7));
}

void PoomFramebuffer::drawChar(char value)
{
    if (value == '\n') {
        cursorX_ = 0;
        cursorY_ += static_cast<int16_t>(8U * textSize_);
        return;
    }

    if (value == '\r') {
        return;
    }

    if (cursorX_ > PoomScreenWidth - static_cast<int16_t>(4U * textSize_)) {
        cursorX_ = 0;
        cursorY_ += static_cast<int16_t>(8U * textSize_);
    }

    for (uint8_t column = 0; column < 3; ++column) {
        drawGlyphColumn(cursorX_ + static_cast<int16_t>(column * textSize_), cursorY_, glyphColumn(value, column), textSize_);
    }

    cursorX_ += static_cast<int16_t>(4U * textSize_);
}

void PoomFramebuffer::drawGlyphColumn(int16_t x, int16_t y, uint8_t bits, uint8_t scale)
{
    for (uint8_t row = 0; row < 5; ++row) {
        if ((bits & (1U << row)) == 0) {
            continue;
        }

        for (uint8_t yy = 0; yy < scale; ++yy) {
            for (uint8_t xx = 0; xx < scale; ++xx) {
                drawPixel(x + xx, y + static_cast<int16_t>(row * scale) + yy, true);
            }
        }
    }
}

uint8_t PoomFramebuffer::glyphColumn(char value, uint8_t column) const
{
    if (value >= 'a' && value <= 'z') {
        value = static_cast<char>(value - ('a' - 'A'));
    }

    static const uint8_t digits[][3] = {
        {0x1F, 0x11, 0x1F},
        {0x00, 0x00, 0x1F},
        {0x1D, 0x15, 0x17},
        {0x15, 0x15, 0x1F},
        {0x07, 0x04, 0x1F},
        {0x17, 0x15, 0x1D},
        {0x1F, 0x15, 0x1D},
        {0x01, 0x01, 0x1F},
        {0x1F, 0x15, 0x1F},
        {0x17, 0x15, 0x1F}
    };
    static const uint8_t letters[][3] = {
        {0x1E, 0x05, 0x1E},
        {0x1F, 0x15, 0x0A},
        {0x0E, 0x11, 0x11},
        {0x1F, 0x11, 0x0E},
        {0x1F, 0x15, 0x11},
        {0x1F, 0x05, 0x01},
        {0x0E, 0x11, 0x1D},
        {0x1F, 0x04, 0x1F},
        {0x11, 0x1F, 0x11},
        {0x08, 0x10, 0x0F},
        {0x1F, 0x04, 0x1B},
        {0x1F, 0x10, 0x10},
        {0x1F, 0x02, 0x1F},
        {0x1F, 0x01, 0x1E},
        {0x0E, 0x11, 0x0E},
        {0x1F, 0x05, 0x02},
        {0x0E, 0x19, 0x1E},
        {0x1F, 0x05, 0x1A},
        {0x12, 0x15, 0x09},
        {0x01, 0x1F, 0x01},
        {0x0F, 0x10, 0x0F},
        {0x07, 0x18, 0x07},
        {0x1F, 0x08, 0x1F},
        {0x1B, 0x04, 0x1B},
        {0x03, 0x1C, 0x03},
        {0x19, 0x15, 0x13}
    };

    if (value >= '0' && value <= '9') {
        return digits[value - '0'][column];
    }
    if (value >= 'A' && value <= 'Z') {
        return letters[value - 'A'][column];
    }
    if (value == '!') {
        static const uint8_t mark[] = {0x00, 0x17, 0x00};
        return mark[column];
    }
    if (value == ':') {
        static const uint8_t mark[] = {0x00, 0x0A, 0x00};
        return mark[column];
    }
    if (value == '.') {
        static const uint8_t mark[] = {0x00, 0x10, 0x00};
        return mark[column];
    }
    if (value == '-') {
        static const uint8_t mark[] = {0x04, 0x04, 0x04};
        return mark[column];
    }

    return 0x00;
}
