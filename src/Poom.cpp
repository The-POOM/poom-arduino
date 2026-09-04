#include "Poom.h"

PoomClass Poom;

bool PoomClass::begin()
{
    graphics_.attach(&framebuffer_);
    buttons_.begin();
    buzzer_.begin();
    leds_.begin();
    framebuffer_.clear();
    frameTimerStarted_ = false;
    return display_.begin();
}

void PoomClass::clear()
{
    framebuffer_.clear();
    display_.clear();
}

void PoomClass::fill(bool on)
{
    framebuffer_.fill(on);
}

void PoomClass::invert()
{
    framebuffer_.invert();
}

void PoomClass::show()
{
    display_.drawBuffer(framebuffer_.data(), framebuffer_.size());
}

uint8_t *PoomClass::buffer()
{
    return framebuffer_.data();
}

const uint8_t *PoomClass::buffer() const
{
    return framebuffer_.data();
}

void PoomClass::drawPixel(int16_t x, int16_t y, bool on)
{
    framebuffer_.drawPixel(x, y, on);
}

void PoomClass::drawHorizontalLine(int16_t x, int16_t y, int16_t width, bool on)
{
    framebuffer_.drawHorizontalLine(x, y, width, on);
}

void PoomClass::drawVerticalLine(int16_t x, int16_t y, int16_t height, bool on)
{
    framebuffer_.drawVerticalLine(x, y, height, on);
}

void PoomClass::fillRect(int16_t x, int16_t y, int16_t width, int16_t height, bool on)
{
    framebuffer_.fillRect(x, y, width, height, on);
}

void PoomClass::drawRect(int16_t x, int16_t y, int16_t width, int16_t height, bool on)
{
    framebuffer_.drawRect(x, y, width, height, on);
}

bool PoomClass::getPixel(int16_t x, int16_t y) const
{
    return framebuffer_.getPixel(x, y);
}

void PoomClass::drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t width, uint8_t height)
{
    graphics_.drawBitmap(x, y, bitmap, width, height);
}

void PoomClass::drawBitmapMasked(
    int16_t x,
    int16_t y,
    const uint8_t *bitmap,
    const uint8_t *mask,
    uint8_t width,
    uint8_t height
)
{
    graphics_.drawBitmapMasked(x, y, bitmap, mask, width, height);
}

void PoomClass::drawSprite(int16_t x, int16_t y, const uint8_t *frames, uint8_t width, uint8_t height, uint8_t frame)
{
    graphics_.drawSprite(x, y, frames, width, height, frame);
}

void PoomClass::drawSpriteMasked(
    int16_t x,
    int16_t y,
    const uint8_t *frames,
    const uint8_t *masks,
    uint8_t width,
    uint8_t height,
    uint8_t frame
)
{
    graphics_.drawSpriteMasked(x, y, frames, masks, width, height, frame);
}

void PoomClass::drawPageBitmap(
    int16_t x,
    int16_t y,
    const uint8_t *bitmap,
    uint8_t width,
    uint8_t height
)
{
    graphics_.drawPageBitmap(x, y, bitmap, width, height);
}

void PoomClass::drawPageBitmapMasked(
    int16_t x,
    int16_t y,
    const uint8_t *bitmap,
    const uint8_t *mask,
    uint8_t width,
    uint8_t height
)
{
    graphics_.drawPageBitmapMasked(x, y, bitmap, mask, width, height);
}

void PoomClass::drawPageSprite(
    int16_t x,
    int16_t y,
    const uint8_t *frames,
    uint8_t width,
    uint8_t height,
    uint8_t frame
)
{
    graphics_.drawPageSprite(x, y, frames, width, height, frame);
}

void PoomClass::setCursor(int16_t x, int16_t y)
{
    framebuffer_.setCursor(x, y);
}

void PoomClass::setTextSize(uint8_t size)
{
    framebuffer_.setTextSize(size);
}

void PoomClass::print(const char *text)
{
    framebuffer_.print(text);
}

void PoomClass::print(const __FlashStringHelper *text)
{
    framebuffer_.print(text);
}

void PoomClass::println(const char *text)
{
    framebuffer_.println(text);
}

void PoomClass::println(const __FlashStringHelper *text)
{
    framebuffer_.println(text);
}

void PoomClass::setFrameRate(uint8_t framesPerSecond)
{
    if (framesPerSecond == 0) {
        framesPerSecond = 1;
    }

    frameIntervalMs_ = 1000U / framesPerSecond;
    if (frameIntervalMs_ == 0) {
        frameIntervalMs_ = 1;
    }
}

bool PoomClass::nextFrame()
{
    update();

    const uint32_t now = millis();
    if (!frameTimerStarted_) {
        frameTimerStarted_ = true;
        lastFrameMs_ = now;
        buttons_.update();
        return true;
    }

    if (static_cast<uint32_t>(now - lastFrameMs_) < frameIntervalMs_) {
        return false;
    }

    lastFrameMs_ = now;
    buttons_.update();
    return true;
}

void PoomClass::update()
{
    buzzer_.update();
}

bool PoomClass::pressed(uint8_t mask) const
{
    return buttons_.pressed(mask);
}

uint8_t PoomClass::buttonState() const
{
    return buttons_.state();
}

bool PoomClass::anyPressed(uint8_t mask) const
{
    return buttons_.anyPressed(mask);
}

bool PoomClass::justPressed(uint8_t mask) const
{
    return buttons_.justPressed(mask);
}

bool PoomClass::justReleased(uint8_t mask) const
{
    return buttons_.justReleased(mask);
}

PoomDisplay &PoomClass::screen()
{
    return display_;
}

PoomFramebuffer &PoomClass::framebuffer()
{
    return framebuffer_;
}

PoomGraphics &PoomClass::graphics()
{
    return graphics_;
}

PoomBuzzer &PoomClass::audio()
{
    return buzzer_;
}

PoomButtons &PoomClass::buttons()
{
    return buttons_;
}

PoomBuzzer &PoomClass::buzzer()
{
    return buzzer_;
}

PoomLeds &PoomClass::leds()
{
    return leds_;
}
