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

    const uint8_t firstPage = static_cast<uint8_t>(startY / 8);
    const uint8_t lastY = static_cast<uint8_t>(endY - 1);
    const uint8_t lastPage = static_cast<uint8_t>(lastY / 8);

    for (uint8_t page = firstPage; page <= lastPage; ++page) {
        uint8_t mask = 0xFF;
        if (page == firstPage) {
            mask &= static_cast<uint8_t>(0xFFU << (startY & 7));
        }
        if (page == lastPage) {
            mask &= static_cast<uint8_t>(0xFFU >> (7U - (lastY & 7U)));
        }

        const size_t index = static_cast<size_t>(page) * PoomScreenWidth + x;
        if (on) {
            pixels_[index] |= mask;
        } else {
            pixels_[index] &= static_cast<uint8_t>(~mask);
        }
    }
}

void PoomFramebuffer::drawLine(
    int16_t x0,
    int16_t y0,
    int16_t x1,
    int16_t y1,
    bool on
)
{
    const bool steep = (y1 > y0 ? y1 - y0 : y0 - y1) >
        (x1 > x0 ? x1 - x0 : x0 - x1);
    if (steep) {
        const int16_t swapX0 = x0;
        x0 = y0;
        y0 = swapX0;
        const int16_t swapX1 = x1;
        x1 = y1;
        y1 = swapX1;
    }

    if (x0 > x1) {
        const int16_t swapX = x0;
        x0 = x1;
        x1 = swapX;
        const int16_t swapY = y0;
        y0 = y1;
        y1 = swapY;
    }

    const int32_t dx = static_cast<int32_t>(x1) - x0;
    const int32_t dy = y1 > y0
        ? static_cast<int32_t>(y1) - y0
        : static_cast<int32_t>(y0) - y1;
    int32_t error = dx / 2;
    const int16_t yStep = y0 < y1 ? 1 : -1;

    for (int32_t drawX = x0; drawX <= x1; ++drawX) {
        if (steep) {
            drawPixel(y0, static_cast<int16_t>(drawX), on);
        } else {
            drawPixel(static_cast<int16_t>(drawX), y0, on);
        }

        error -= dy;
        if (error < 0) {
            y0 = static_cast<int16_t>(y0 + yStep);
            error += dx;
        }
    }
}

void PoomFramebuffer::drawCircle(int16_t x0, int16_t y0, int16_t radius, bool on)
{
    if (radius < 0) {
        return;
    }

    int32_t f = 1 - radius;
    int32_t deltaX = 1;
    int32_t deltaY = -2L * radius;
    int16_t x = 0;
    int16_t y = radius;

    drawPixel(x0, static_cast<int16_t>(y0 + radius), on);
    drawPixel(x0, static_cast<int16_t>(y0 - radius), on);
    drawPixel(static_cast<int16_t>(x0 + radius), y0, on);
    drawPixel(static_cast<int16_t>(x0 - radius), y0, on);

    while (x < y) {
        if (f >= 0) {
            --y;
            deltaY += 2;
            f += deltaY;
        }

        ++x;
        deltaX += 2;
        f += deltaX;

        drawPixel(static_cast<int16_t>(x0 + x), static_cast<int16_t>(y0 + y), on);
        drawPixel(static_cast<int16_t>(x0 - x), static_cast<int16_t>(y0 + y), on);
        drawPixel(static_cast<int16_t>(x0 + x), static_cast<int16_t>(y0 - y), on);
        drawPixel(static_cast<int16_t>(x0 - x), static_cast<int16_t>(y0 - y), on);
        drawPixel(static_cast<int16_t>(x0 + y), static_cast<int16_t>(y0 + x), on);
        drawPixel(static_cast<int16_t>(x0 - y), static_cast<int16_t>(y0 + x), on);
        drawPixel(static_cast<int16_t>(x0 + y), static_cast<int16_t>(y0 - x), on);
        drawPixel(static_cast<int16_t>(x0 - y), static_cast<int16_t>(y0 - x), on);
    }
}

void PoomFramebuffer::fillCircle(int16_t x, int16_t y, int16_t radius, bool on)
{
    if (radius < 0) {
        return;
    }

    drawVerticalLine(x, static_cast<int16_t>(y - radius), static_cast<int16_t>(2 * radius + 1), on);
    fillCircleHelper(x, y, radius, 3, 0, on);
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

    const size_t clippedX = static_cast<size_t>(startX);
    const size_t clippedWidth = static_cast<size_t>(endX - startX);
    const uint8_t firstPage = static_cast<uint8_t>(startY / 8);
    const uint8_t lastY = static_cast<uint8_t>(endY - 1);
    const uint8_t lastPage = static_cast<uint8_t>(lastY / 8);

    for (uint8_t page = firstPage; page <= lastPage; ++page) {
        uint8_t mask = 0xFF;
        if (page == firstPage) {
            mask &= static_cast<uint8_t>(0xFFU << (startY & 7));
        }
        if (page == lastPage) {
            mask &= static_cast<uint8_t>(0xFFU >> (7U - (lastY & 7U)));
        }

        uint8_t *row = pixels_ + static_cast<size_t>(page) * PoomScreenWidth + clippedX;
        if (mask == 0xFF) {
            memset(row, on ? 0xFF : 0x00, clippedWidth);
            continue;
        }

        for (size_t drawX = 0; drawX < clippedWidth; ++drawX) {
            if (on) {
                row[drawX] |= mask;
            } else {
                row[drawX] &= static_cast<uint8_t>(~mask);
            }
        }
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

void PoomFramebuffer::drawRoundRect(
    int16_t x,
    int16_t y,
    int16_t width,
    int16_t height,
    int16_t radius,
    bool on
)
{
    if (width <= 0 || height <= 0 || radius < 0) {
        return;
    }

    const int16_t maximumRadius = static_cast<int16_t>((width < height ? width : height) / 2);
    if (radius > maximumRadius) {
        radius = maximumRadius;
    }

    drawHorizontalLine(static_cast<int16_t>(x + radius), y, static_cast<int16_t>(width - 2 * radius), on);
    drawHorizontalLine(
        static_cast<int16_t>(x + radius),
        static_cast<int16_t>(y + height - 1),
        static_cast<int16_t>(width - 2 * radius),
        on
    );
    drawVerticalLine(x, static_cast<int16_t>(y + radius), static_cast<int16_t>(height - 2 * radius), on);
    drawVerticalLine(
        static_cast<int16_t>(x + width - 1),
        static_cast<int16_t>(y + radius),
        static_cast<int16_t>(height - 2 * radius),
        on
    );

    drawCircleHelper(static_cast<int16_t>(x + radius), static_cast<int16_t>(y + radius), radius, 1, on);
    drawCircleHelper(
        static_cast<int16_t>(x + width - radius - 1),
        static_cast<int16_t>(y + radius),
        radius,
        2,
        on
    );
    drawCircleHelper(
        static_cast<int16_t>(x + width - radius - 1),
        static_cast<int16_t>(y + height - radius - 1),
        radius,
        4,
        on
    );
    drawCircleHelper(
        static_cast<int16_t>(x + radius),
        static_cast<int16_t>(y + height - radius - 1),
        radius,
        8,
        on
    );
}

void PoomFramebuffer::fillRoundRect(
    int16_t x,
    int16_t y,
    int16_t width,
    int16_t height,
    int16_t radius,
    bool on
)
{
    if (width <= 0 || height <= 0 || radius < 0) {
        return;
    }

    const int16_t maximumRadius = static_cast<int16_t>((width < height ? width : height) / 2);
    if (radius > maximumRadius) {
        radius = maximumRadius;
    }

    fillRect(static_cast<int16_t>(x + radius), y, static_cast<int16_t>(width - 2 * radius), height, on);
    const int16_t delta = static_cast<int16_t>(height - 2 * radius - 1);
    fillCircleHelper(
        static_cast<int16_t>(x + width - radius - 1),
        static_cast<int16_t>(y + radius),
        radius,
        1,
        delta,
        on
    );
    fillCircleHelper(
        static_cast<int16_t>(x + radius),
        static_cast<int16_t>(y + radius),
        radius,
        2,
        delta,
        on
    );
}

void PoomFramebuffer::drawTriangle(
    int16_t x0,
    int16_t y0,
    int16_t x1,
    int16_t y1,
    int16_t x2,
    int16_t y2,
    bool on
)
{
    drawLine(x0, y0, x1, y1, on);
    drawLine(x1, y1, x2, y2, on);
    drawLine(x2, y2, x0, y0, on);
}

void PoomFramebuffer::fillTriangle(
    int16_t x0,
    int16_t y0,
    int16_t x1,
    int16_t y1,
    int16_t x2,
    int16_t y2,
    bool on
)
{
    if (y0 > y1) {
        int16_t swap = y0; y0 = y1; y1 = swap;
        swap = x0; x0 = x1; x1 = swap;
    }
    if (y1 > y2) {
        int16_t swap = y1; y1 = y2; y2 = swap;
        swap = x1; x1 = x2; x2 = swap;
    }
    if (y0 > y1) {
        int16_t swap = y0; y0 = y1; y1 = swap;
        swap = x0; x0 = x1; x1 = swap;
    }

    if (y0 == y2) {
        int16_t left = x0;
        int16_t right = x0;
        if (x1 < left) left = x1; else if (x1 > right) right = x1;
        if (x2 < left) left = x2; else if (x2 > right) right = x2;
        drawHorizontalLine(left, y0, static_cast<int16_t>(right - left + 1), on);
        return;
    }

    const int32_t dx01 = static_cast<int32_t>(x1) - x0;
    const int32_t dy01 = static_cast<int32_t>(y1) - y0;
    const int32_t dx02 = static_cast<int32_t>(x2) - x0;
    const int32_t dy02 = static_cast<int32_t>(y2) - y0;
    const int32_t dx12 = static_cast<int32_t>(x2) - x1;
    const int32_t dy12 = static_cast<int32_t>(y2) - y1;
    int32_t accumulatorA = 0;
    int32_t accumulatorB = 0;
    int16_t drawY;
    const int16_t last = y1 == y2 ? y1 : static_cast<int16_t>(y1 - 1);

    for (drawY = y0; drawY <= last; ++drawY) {
        int16_t left = static_cast<int16_t>(x0 + accumulatorA / dy01);
        int16_t right = static_cast<int16_t>(x0 + accumulatorB / dy02);
        accumulatorA += dx01;
        accumulatorB += dx02;
        if (left > right) {
            const int16_t swap = left; left = right; right = swap;
        }
        drawHorizontalLine(left, drawY, static_cast<int16_t>(right - left + 1), on);
    }

    accumulatorA = dx12 * (drawY - y1);
    accumulatorB = dx02 * (drawY - y0);
    for (; drawY <= y2; ++drawY) {
        int16_t left = static_cast<int16_t>(x1 + accumulatorA / dy12);
        int16_t right = static_cast<int16_t>(x0 + accumulatorB / dy02);
        accumulatorA += dx12;
        accumulatorB += dx02;
        if (left > right) {
            const int16_t swap = left; left = right; right = swap;
        }
        drawHorizontalLine(left, drawY, static_cast<int16_t>(right - left + 1), on);
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

void PoomFramebuffer::drawCircleHelper(
    int16_t x0,
    int16_t y0,
    int16_t radius,
    uint8_t corners,
    bool on
)
{
    int32_t f = 1 - radius;
    int32_t deltaX = 1;
    int32_t deltaY = -2L * radius;
    int16_t x = 0;
    int16_t y = radius;

    while (x < y) {
        if (f >= 0) {
            --y;
            deltaY += 2;
            f += deltaY;
        }
        ++x;
        deltaX += 2;
        f += deltaX;

        if ((corners & 0x4U) != 0) {
            drawPixel(static_cast<int16_t>(x0 + x), static_cast<int16_t>(y0 + y), on);
            drawPixel(static_cast<int16_t>(x0 + y), static_cast<int16_t>(y0 + x), on);
        }
        if ((corners & 0x2U) != 0) {
            drawPixel(static_cast<int16_t>(x0 + x), static_cast<int16_t>(y0 - y), on);
            drawPixel(static_cast<int16_t>(x0 + y), static_cast<int16_t>(y0 - x), on);
        }
        if ((corners & 0x8U) != 0) {
            drawPixel(static_cast<int16_t>(x0 - y), static_cast<int16_t>(y0 + x), on);
            drawPixel(static_cast<int16_t>(x0 - x), static_cast<int16_t>(y0 + y), on);
        }
        if ((corners & 0x1U) != 0) {
            drawPixel(static_cast<int16_t>(x0 - y), static_cast<int16_t>(y0 - x), on);
            drawPixel(static_cast<int16_t>(x0 - x), static_cast<int16_t>(y0 - y), on);
        }
    }
}

void PoomFramebuffer::fillCircleHelper(
    int16_t x0,
    int16_t y0,
    int16_t radius,
    uint8_t sides,
    int16_t delta,
    bool on
)
{
    int32_t f = 1 - radius;
    int32_t deltaX = 1;
    int32_t deltaY = -2L * radius;
    int16_t x = 0;
    int16_t y = radius;

    while (x < y) {
        if (f >= 0) {
            --y;
            deltaY += 2;
            f += deltaY;
        }
        ++x;
        deltaX += 2;
        f += deltaX;

        if ((sides & 0x1U) != 0) {
            drawVerticalLine(
                static_cast<int16_t>(x0 + x),
                static_cast<int16_t>(y0 - y),
                static_cast<int16_t>(2 * y + 1 + delta),
                on
            );
            drawVerticalLine(
                static_cast<int16_t>(x0 + y),
                static_cast<int16_t>(y0 - x),
                static_cast<int16_t>(2 * x + 1 + delta),
                on
            );
        }
        if ((sides & 0x2U) != 0) {
            drawVerticalLine(
                static_cast<int16_t>(x0 - x),
                static_cast<int16_t>(y0 - y),
                static_cast<int16_t>(2 * y + 1 + delta),
                on
            );
            drawVerticalLine(
                static_cast<int16_t>(x0 - y),
                static_cast<int16_t>(y0 - x),
                static_cast<int16_t>(2 * x + 1 + delta),
                on
            );
        }
    }
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
