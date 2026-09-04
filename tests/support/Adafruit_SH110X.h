#ifndef TEST_ADAFRUIT_SH110X_H
#define TEST_ADAFRUIT_SH110X_H

#include <Arduino.h>
#include <Wire.h>
#include <string.h>

constexpr uint8_t SH110X_BLACK = 0;
constexpr uint8_t SH110X_WHITE = 1;
constexpr uint8_t SH110X_DISPLAYON = 0xAF;

class Adafruit_SH1106G
{
public:
    Adafruit_SH1106G(uint8_t, uint8_t, TwoWire *, int8_t) { }

    bool begin(uint8_t, bool) { return true; }
    void setRotation(uint8_t) { }
    void oled_command(uint8_t) { }
    void clearDisplay() { memset(buffer_, 0, sizeof(buffer_)); }
    void display() { }
    void drawPixel(int16_t x, int16_t y, uint16_t color)
    {
        if (x < 0 || x >= 128 || y < 0 || y >= 64) return;
        const size_t index = static_cast<size_t>(y / 8) * 128U + x;
        const uint8_t mask = static_cast<uint8_t>(1U << (y & 7));
        if (color) buffer_[index] |= mask;
        else buffer_[index] &= static_cast<uint8_t>(~mask);
    }
    uint8_t *getBuffer() { return buffer_; }
    void setTextSize(uint8_t) { }
    void setTextColor(uint16_t) { }
    void setCursor(int16_t, int16_t) { }
    void print(const char *) { }
    void print(const __FlashStringHelper *) { }
    void println(const char *) { }
    void println(const __FlashStringHelper *) { }

private:
    uint8_t buffer_[128 * 64 / 8] = {};
};

#endif
