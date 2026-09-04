#include "Poom.h"

#if defined(ARDUINO_ARCH_ESP32)
#include <esp_ota_ops.h>
#include <esp_system.h>
#endif

namespace
{
constexpr uint8_t PoomExitChord = PoomButtonA | PoomButtonB | PoomButtonDown;
constexpr uint32_t PoomExitHoldMs = 2000;
constexpr uint8_t PoomMaximumCatchUpFrames = 3;
}

PoomClass Poom;

bool PoomClass::begin()
{
    graphics_.attach(&framebuffer_);
    buttons_.begin();
    buzzer_.begin();
    leds_.begin();
    framebuffer_.clear();
    frameTimerStarted_ = false;
    frameMetricsStarted_ = false;
    frameCount_ = 0;
    droppedFrameCount_ = 0;
    lastFrameLatenessUs_ = 0;
    frameMetricsCount_ = 0;
    measuredFrameRate_ = 0;
    frameRemainderAccumulator_ = 0;
    exitChordHeld_ = false;
#if defined(ARDUINO_ARCH_ESP32)
    const esp_partition_t *runningPartition = esp_ota_get_running_partition();
    runningFromLauncher_ = runningPartition &&
        runningPartition->type == ESP_PARTITION_TYPE_APP &&
        runningPartition->subtype == ESP_PARTITION_SUBTYPE_APP_OTA_1;
#else
    runningFromLauncher_ = false;
#endif
    exitChordEnabled_ = runningFromLauncher_;
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

void PoomClass::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, bool on)
{
    framebuffer_.drawLine(x0, y0, x1, y1, on);
}

void PoomClass::drawCircle(int16_t x, int16_t y, int16_t radius, bool on)
{
    framebuffer_.drawCircle(x, y, radius, on);
}

void PoomClass::fillCircle(int16_t x, int16_t y, int16_t radius, bool on)
{
    framebuffer_.fillCircle(x, y, radius, on);
}

void PoomClass::fillRect(int16_t x, int16_t y, int16_t width, int16_t height, bool on)
{
    framebuffer_.fillRect(x, y, width, height, on);
}

void PoomClass::drawRect(int16_t x, int16_t y, int16_t width, int16_t height, bool on)
{
    framebuffer_.drawRect(x, y, width, height, on);
}

void PoomClass::drawRoundRect(
    int16_t x,
    int16_t y,
    int16_t width,
    int16_t height,
    int16_t radius,
    bool on
)
{
    framebuffer_.drawRoundRect(x, y, width, height, radius, on);
}

void PoomClass::fillRoundRect(
    int16_t x,
    int16_t y,
    int16_t width,
    int16_t height,
    int16_t radius,
    bool on
)
{
    framebuffer_.fillRoundRect(x, y, width, height, radius, on);
}

void PoomClass::drawTriangle(
    int16_t x0,
    int16_t y0,
    int16_t x1,
    int16_t y1,
    int16_t x2,
    int16_t y2,
    bool on
)
{
    framebuffer_.drawTriangle(x0, y0, x1, y1, x2, y2, on);
}

void PoomClass::fillTriangle(
    int16_t x0,
    int16_t y0,
    int16_t x1,
    int16_t y1,
    int16_t x2,
    int16_t y2,
    bool on
)
{
    framebuffer_.fillTriangle(x0, y0, x1, y1, x2, y2, on);
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

void PoomClass::drawPageSpriteMasked(
    int16_t x,
    int16_t y,
    const uint8_t *frames,
    const uint8_t *masks,
    uint8_t width,
    uint8_t height,
    uint8_t frame
)
{
    graphics_.drawPageSpriteMasked(x, y, frames, masks, width, height, frame);
}

void PoomClass::drawPageSpriteMasked(
    int16_t x,
    int16_t y,
    const uint8_t *frames,
    const uint8_t *masks,
    uint8_t width,
    uint8_t height,
    uint8_t imageFrame,
    uint8_t maskFrame
)
{
    graphics_.drawPageSpriteMasked(
        x, y, frames, masks, width, height, imageFrame, maskFrame
    );
}

void PoomClass::drawPageSpriteInterleavedMasked(
    int16_t x,
    int16_t y,
    const uint8_t *frames,
    uint8_t width,
    uint8_t height,
    uint8_t frame
)
{
    graphics_.drawPageSpriteInterleavedMasked(x, y, frames, width, height, frame);
}

void PoomClass::erasePageSprite(
    int16_t x,
    int16_t y,
    const uint8_t *frames,
    uint8_t width,
    uint8_t height,
    uint8_t frame
)
{
    graphics_.erasePageSprite(x, y, frames, width, height, frame);
}

void PoomClass::overwritePageSprite(
    int16_t x,
    int16_t y,
    const uint8_t *frames,
    uint8_t width,
    uint8_t height,
    uint8_t frame
)
{
    graphics_.overwritePageSprite(x, y, frames, width, height, frame);
}

void PoomClass::drawPageAsset(int16_t x, int16_t y, const uint8_t *asset, uint8_t frame)
{
    graphics_.drawPageAsset(x, y, asset, frame);
}

void PoomClass::drawPageAssetMasked(
    int16_t x,
    int16_t y,
    const uint8_t *asset,
    const uint8_t *mask,
    uint8_t imageFrame,
    uint8_t maskFrame
)
{
    graphics_.drawPageAssetMasked(x, y, asset, mask, imageFrame, maskFrame);
}

void PoomClass::drawPageAssetInterleavedMasked(
    int16_t x,
    int16_t y,
    const uint8_t *asset,
    uint8_t frame
)
{
    graphics_.drawPageAssetInterleavedMasked(x, y, asset, frame);
}

void PoomClass::erasePageAsset(int16_t x, int16_t y, const uint8_t *asset, uint8_t frame)
{
    graphics_.erasePageAsset(x, y, asset, frame);
}

void PoomClass::overwritePageAsset(int16_t x, int16_t y, const uint8_t *asset, uint8_t frame)
{
    graphics_.overwritePageAsset(x, y, asset, frame);
}

void PoomClass::drawCompressed(int16_t x, int16_t y, const uint8_t *bitmap, bool on)
{
    graphics_.drawCompressed(x, y, bitmap, on);
}

void PoomClass::drawCompressedMirror(
    int16_t x,
    int16_t y,
    const uint8_t *bitmap,
    bool on,
    bool mirror
)
{
    graphics_.drawCompressedMirror(x, y, bitmap, on, mirror);
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

    targetFrameRate_ = framesPerSecond;
    framePeriodUs_ = 1000000UL / targetFrameRate_;
    framePeriodRemainder_ = 1000000UL % targetFrameRate_;
    if (framePeriodUs_ == 0) {
        framePeriodUs_ = 1;
        framePeriodRemainder_ = 0;
    }

    frameTimerStarted_ = false;
    frameRemainderAccumulator_ = 0;
    frameMetricsStarted_ = false;
    frameMetricsCount_ = 0;
    measuredFrameRate_ = 0;
}

bool PoomClass::nextFrame()
{
    update();

    const uint32_t nowUs = micros();
    if (!frameTimerStarted_) {
        frameTimerStarted_ = true;
        nextFrameDeadlineUs_ = nowUs;
        advanceFrameDeadline();
        lastFrameLatenessUs_ = 0;
        buttons_.update();
        updateExitChord(millis());
        ++frameCount_;
        recordAcceptedFrame(nowUs);
        return true;
    }

    if (static_cast<int32_t>(nowUs - nextFrameDeadlineUs_) < 0) {
        return false;
    }

    lastFrameLatenessUs_ = nowUs - nextFrameDeadlineUs_;
    const uint32_t maximumCatchUpUs = framePeriodUs_ * PoomMaximumCatchUpFrames;
    if (lastFrameLatenessUs_ > maximumCatchUpUs) {
        droppedFrameCount_ += lastFrameLatenessUs_ / framePeriodUs_;
        nextFrameDeadlineUs_ = nowUs;
        frameRemainderAccumulator_ = 0;
    }
    advanceFrameDeadline();

    buttons_.update();
    updateExitChord(millis());
    ++frameCount_;
    recordAcceptedFrame(nowUs);
    return true;
}

uint32_t PoomClass::frameCount() const
{
    return frameCount_;
}

uint8_t PoomClass::targetFrameRate() const
{
    return targetFrameRate_;
}

uint16_t PoomClass::measuredFrameRate() const
{
    return measuredFrameRate_;
}

uint32_t PoomClass::droppedFrameCount() const
{
    return droppedFrameCount_;
}

uint32_t PoomClass::lastFrameLatenessMicros() const
{
    return lastFrameLatenessUs_;
}

bool PoomClass::everyFrames(uint16_t interval) const
{
    return interval != 0 && (frameCount_ % interval) == 0;
}

void PoomClass::update()
{
    buzzer_.update();
}

void PoomClass::advanceFrameDeadline()
{
    nextFrameDeadlineUs_ += framePeriodUs_;
    frameRemainderAccumulator_ += framePeriodRemainder_;
    if (frameRemainderAccumulator_ >= targetFrameRate_) {
        ++nextFrameDeadlineUs_;
        frameRemainderAccumulator_ -= targetFrameRate_;
    }
}

void PoomClass::recordAcceptedFrame(uint32_t nowUs)
{
    if (!frameMetricsStarted_) {
        frameMetricsStarted_ = true;
        frameMetricsStartUs_ = nowUs;
        frameMetricsCount_ = 0;
        return;
    }

    ++frameMetricsCount_;
    const uint32_t elapsedUs = nowUs - frameMetricsStartUs_;
    if (elapsedUs >= 1000000UL) {
        measuredFrameRate_ = static_cast<uint16_t>(
            (static_cast<uint32_t>(frameMetricsCount_) * 1000000UL + elapsedUs / 2U) /
            elapsedUs
        );
        frameMetricsStartUs_ = nowUs;
        frameMetricsCount_ = 0;
    }
}

bool PoomClass::runningFromLauncher() const
{
    return runningFromLauncher_;
}

void PoomClass::enableExitChord(bool enabled)
{
    exitChordEnabled_ = enabled && runningFromLauncher_;
    exitChordHeld_ = false;
}

bool PoomClass::exitChordEnabled() const
{
    return exitChordEnabled_;
}

bool PoomClass::returnToLauncher()
{
    if (!runningFromLauncher_) {
        return false;
    }

    buzzer_.stop();
#if defined(ARDUINO_ARCH_ESP32)
    esp_restart();
#endif
    return true;
}

void PoomClass::updateExitChord(uint32_t now)
{
    if (!exitChordEnabled_ || !buttons_.pressed(PoomExitChord)) {
        exitChordHeld_ = false;
        return;
    }

    if (!exitChordHeld_) {
        exitChordHeld_ = true;
        exitChordStartedMs_ = now;
        return;
    }

    if (static_cast<uint32_t>(now - exitChordStartedMs_) >= PoomExitHoldMs) {
        returnToLauncher();
    }
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

PoomStorage &PoomClass::storage()
{
    return storage_;
}
