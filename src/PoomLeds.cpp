#include "PoomLeds.h"

void PoomLeds::begin()
{
#if POOM_USE_NEOPIXEL
    if (initialized_) {
        return;
    }

    strip_.begin();
    strip_.clear();
    strip_.show();
    initialized_ = true;
#endif
}

void PoomLeds::setBrightness(uint8_t brightness)
{
#if POOM_USE_NEOPIXEL
    begin();
    strip_.setBrightness(brightness);
#else
    (void)brightness;
#endif
}

void PoomLeds::setColor(uint8_t red, uint8_t green, uint8_t blue)
{
#if POOM_USE_NEOPIXEL
    begin();
    const uint32_t color = strip_.Color(red, green, blue);
    for (uint16_t i = 0; i < strip_.numPixels(); ++i) {
        strip_.setPixelColor(i, color);
    }
    strip_.show();
#else
    (void)red;
    (void)green;
    (void)blue;
#endif
}

void PoomLeds::setPixelColor(uint16_t index, uint8_t red, uint8_t green, uint8_t blue)
{
#if POOM_USE_NEOPIXEL
    begin();
    if (index >= strip_.numPixels()) {
        return;
    }

    strip_.setPixelColor(index, strip_.Color(red, green, blue));
#else
    (void)index;
    (void)red;
    (void)green;
    (void)blue;
#endif
}

void PoomLeds::off()
{
#if POOM_USE_NEOPIXEL
    begin();
    strip_.clear();
    strip_.show();
#endif
}

void PoomLeds::show()
{
#if POOM_USE_NEOPIXEL
    begin();
    strip_.show();
#endif
}

uint16_t PoomLeds::count() const
{
    return POOM_LED_COUNT;
}
