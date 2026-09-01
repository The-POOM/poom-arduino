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
