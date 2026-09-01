#ifndef POOM_LEDS_H
#define POOM_LEDS_H

/**
 * @file PoomLeds.h
 * @brief Onboard RGB LED helpers for Poom.
 */

#include <Adafruit_NeoPixel.h>
#include <stdint.h>

#include "PoomBoardConfig.h"

/**
 * @brief Helper for the onboard WS2812/NeoPixel LEDs.
 */
class PoomLeds
{
public:
    /** @brief Initialize the LED strip. */
    void begin();

    /**
     * @brief Set global LED brightness.
     * @param brightness Brightness from 0 to 255.
     */
    void setBrightness(uint8_t brightness);

    /**
     * @brief Set all LEDs to the same RGB color and show immediately.
     * @param red Red channel, 0 to 255.
     * @param green Green channel, 0 to 255.
     * @param blue Blue channel, 0 to 255.
     */
    void setColor(uint8_t red, uint8_t green, uint8_t blue);

    /**
     * @brief Set one LED color and show immediately.
     * @param index LED index, starting at 0.
     * @param red Red channel, 0 to 255.
     * @param green Green channel, 0 to 255.
     * @param blue Blue channel, 0 to 255.
     */
    void setPixelColor(uint16_t index, uint8_t red, uint8_t green, uint8_t blue);

    /** @brief Turn off all LEDs and show immediately. */
    void off();

    /** @brief Flush pending LED data to the strip. */
    void show();

    /** @return Number of configured LEDs. */
    uint16_t count() const;

private:
    bool initialized_ = false;
    Adafruit_NeoPixel strip_{POOM_LED_COUNT, POOM_NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800};
};

#endif
