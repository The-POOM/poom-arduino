#include "PoomDisplay.h"

bool PoomDisplay::begin()
{
    if (initialized_) {
        return true;
    }

    Wire.begin(POOM_I2C_SDA_PIN, POOM_I2C_SCL_PIN);

    if (!applyInitSequence()) {
        return false;
    }

    if (!oled_.begin(POOM_DISPLAY_I2C_ADDR, true)) {
        return false;
    }

    if (!applyInitSequence()) {
        return false;
    }

    oled_.setRotation(POOM_DISPLAY_ROTATION);
    oled_.oled_command(SH110X_DISPLAYON);
    oled_.clearDisplay();
    applyTextDefaults();
    oled_.display();

    initialized_ = true;
    return true;
}

void PoomDisplay::clear()
{
    oled_.clearDisplay();
    applyTextDefaults();
}

void PoomDisplay::show()
{
    oled_.display();
}

void PoomDisplay::drawPixel(int16_t x, int16_t y, bool on)
{
    oled_.drawPixel(x, y, on ? SH110X_WHITE : SH110X_BLACK);
}

void PoomDisplay::setCursor(int16_t x, int16_t y)
{
    oled_.setCursor(x, y);
}

void PoomDisplay::setTextSize(uint8_t size)
{
    oled_.setTextSize(size);
    oled_.setTextColor(SH110X_WHITE);
}

void PoomDisplay::print(const char *text)
{
    oled_.print(text);
}

void PoomDisplay::print(const __FlashStringHelper *text)
{
    oled_.print(text);
}

void PoomDisplay::println(const char *text)
{
    oled_.println(text);
}

void PoomDisplay::println(const __FlashStringHelper *text)
{
    oled_.println(text);
}

void PoomDisplay::drawBuffer(const uint8_t *buffer, size_t size)
{
    if (!buffer || size < PoomFramebufferSize) {
        return;
    }

    oled_.clearDisplay();

    for (uint8_t y = 0; y < PoomScreenHeight; ++y) {
        const uint8_t row = y >> 3;
        const uint8_t bit = 1U << (y & 7);

        for (uint8_t x = 0; x < PoomScreenWidth; ++x) {
            const size_t index = static_cast<size_t>(row) * PoomScreenWidth + x;
            if ((buffer[index] & bit) == 0) {
                continue;
            }

            int16_t mappedX = static_cast<int16_t>(x) + POOM_DISPLAY_X_SHIFT;
            if (mappedX < 0) {
                mappedX += PoomScreenWidth;
            } else if (mappedX >= PoomScreenWidth) {
                mappedX -= PoomScreenWidth;
            }

            oled_.drawPixel(mappedX, y, SH110X_WHITE);
        }
    }

    oled_.display();
}

uint8_t PoomDisplay::width() const
{
    return PoomScreenWidth;
}

uint8_t PoomDisplay::height() const
{
    return PoomScreenHeight;
}

Adafruit_SH1106G &PoomDisplay::native()
{
    return oled_;
}

bool PoomDisplay::applyInitSequence()
{
    const uint8_t mux = (PoomScreenHeight == 64) ? 0x3F : 0x1F;
    const uint8_t compPins = (PoomScreenHeight == 64) ? 0x12 : 0x02;
    const uint8_t initSeq[] = {
        0x00,
        0xAE,
        0xA8, mux,
        0xD3, 0x00,
        0x40,
        0xA1,
        0xC8,
        0xD5, 0x80,
        0xDA, compPins,
        0x81, 0xFF,
        0xA4,
        0xDB, 0x40,
        0x20, 0x02,
        0x00, 0x10,
        0x8D, 0x14,
        0x2E,
        0xA6,
        0xAF
    };

    Wire.beginTransmission(POOM_DISPLAY_I2C_ADDR);
    Wire.write(initSeq, sizeof(initSeq));
    return Wire.endTransmission() == 0;
}

void PoomDisplay::applyTextDefaults()
{
    oled_.setTextSize(1);
    oled_.setTextColor(SH110X_WHITE);
    oled_.setCursor(0, 0);
}
