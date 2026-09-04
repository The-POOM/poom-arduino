#pragma once

#include "../PoomGameCompat.h"

class PoomAudioCompat {

  public:

    static void begin();
    static bool enabled();
    static void toggle();
    static void on();
    static void off();

};

class Arduboy2Ext {

  public:

    Arduboy2Ext();

    PoomAudioCompat audio;
    uint8_t *sBuffer;
    mutable uint16_t frameCount = 0;

    void boot();
    void systemButtons();
    bool nextFrame();
    void pollButtons();
    void setFrameRate(uint8_t rate);
    void invert(bool inverted);
    void fillRect(int16_t x, int16_t y, uint8_t width, uint8_t height, uint8_t color = WHITE);

    void setRGBled(uint8_t red, uint8_t green, uint8_t blue);
    void setRGBled(uint8_t color, uint8_t value);
    void setRGBledRedOn();
    void setRGBledBlueOn();
    void setRGBledRedOff();
    void setRGBledGreenOff();
    void setRGBledBlueOff();
    void setRGBledGreenOn();

    static bool collide(const Point &point, const Rect &rect);
    static bool collide(const Rect &first, const Rect &second);

    uint8_t justPressedButtons() const;
    uint8_t pressedButtons() const;
    uint16_t getFrameCount() const;
    void setFrameCount(uint16_t val) const;
    
    uint8_t getFrameCount(uint8_t mod, int8_t offset = 0) const;
    bool getFrameCountHalf(uint8_t mod) const;
    bool isFrameCount(uint8_t mod) const;
    bool isFrameCount(uint8_t mod, uint8_t val) const;
    
    void clearButtonState();
    void resetFrameCount();
    uint8_t randomLFSR(uint8_t min, uint8_t max);

  private:

    uint8_t currentButtonState = 0;
    uint8_t previousButtonState = 0;
    uint8_t ledRed_ = 0;
    uint8_t ledGreen_ = 0;
    uint8_t ledBlue_ = 0;

    void showLed();
        
};
