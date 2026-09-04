#include "PoomDisplay.h"

#include <string.h>

bool PoomDisplay::begin()
{
    if (initialized_) {
        return true;
    }

    Wire.begin(POOM_I2C_SDA_PIN, POOM_I2C_SCL_PIN);
    Wire.setClock(i2cClockHz_);

    if (!oled_.begin(POOM_DISPLAY_I2C_ADDR, true)) {
        return false;
    }

#if defined(WIRE_HAS_BUFFER_SIZE)
    // One control byte plus one complete 128-byte OLED page. This lets the
    // ESP32 Wire implementation send a page in a single data transaction.
    const size_t wireBufferSize = Wire.setBufferSize(PoomScreenWidth + 1U);
    if (wireBufferSize > 1U) {
        i2cDataCapacity_ = wireBufferSize - 1U;
        if (i2cDataCapacity_ > PoomScreenWidth) {
            i2cDataCapacity_ = PoomScreenWidth;
        }
    }
#endif

    oled_.setRotation(POOM_DISPLAY_ROTATION);
    oled_.clearDisplay();
    applyTextDefaults();
    oled_.display();
    Wire.setClock(i2cClockHz_);

    shadowValid_ = false;
    resetMetrics();
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
    const uint32_t startedMicros = micros();
    size_t dataBytesSent = 0;
    bool directError = false;

    if (POOM_DISPLAY_ROTATION == 0) {
        if (drawBufferDirect(oled_.getBuffer(), dataBytesSent)) {
            recordPresentation(dataBytesSent, micros() - startedMicros, false);
            return;
        }
        directError = true;
    }

    Wire.setClock(i2cClockHz_);
    oled_.display();
    shadowValid_ = false;
    recordPresentation(PoomFramebufferSize, micros() - startedMicros, directError);
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

    const uint32_t startedMicros = micros();
    size_t dataBytesSent = 0;
    bool directError = false;

    if (POOM_DISPLAY_ROTATION == 0) {
        uint8_t *nativeBuffer = oled_.getBuffer();
        if (buffer != nativeBuffer) {
            memcpy(nativeBuffer, buffer, PoomFramebufferSize);
        }
        if (drawBufferDirect(nativeBuffer, dataBytesSent)) {
            recordPresentation(dataBytesSent, micros() - startedMicros, false);
            return;
        }
        directError = true;
    }

    drawBufferWithRotation(buffer);
    Wire.setClock(i2cClockHz_);
    oled_.display();
    shadowValid_ = false;
    recordPresentation(PoomFramebufferSize, micros() - startedMicros, directError);
}

void PoomDisplay::setI2CClock(uint32_t clockHz)
{
    if (clockHz == 0) {
        return;
    }

    i2cClockHz_ = clockHz;
    if (initialized_) {
        Wire.setClock(i2cClockHz_);
    }
}

uint32_t PoomDisplay::i2cClock() const
{
    return i2cClockHz_;
}

const PoomDisplayMetrics &PoomDisplay::metrics() const
{
    return metrics_;
}

void PoomDisplay::resetMetrics()
{
    metrics_ = PoomDisplayMetrics{};
    metricsWindowStarted_ = false;
    metricsWindowStartMs_ = 0;
    windowPresentCount_ = 0;
    windowTransferCount_ = 0;
}

void PoomDisplay::invalidateShadow()
{
    shadowValid_ = false;
}

bool PoomDisplay::drawBufferDirect(const uint8_t *buffer, size_t &dataBytesSent)
{
    // Starting another range costs a command transaction and a data control
    // byte. Sending a short unchanged gap is cheaper than reopening the range.
    constexpr uint8_t MergeGapBytes = 7;

    if (!buffer) {
        return false;
    }

    int16_t shift = static_cast<int16_t>(POOM_DISPLAY_X_SHIFT % PoomScreenWidth);
    if (shift < 0) {
        shift = static_cast<int16_t>(shift + PoomScreenWidth);
    }

    uint8_t shiftedPage[PoomScreenWidth];
    Wire.setClock(i2cClockHz_);

    for (uint8_t page = 0; page < PoomScreenHeight / 8; ++page) {
        const uint8_t *source = buffer + static_cast<size_t>(page) * PoomScreenWidth;
        const uint8_t *pageData = source;

        if (shift != 0) {
            for (uint8_t sourceX = 0; sourceX < PoomScreenWidth; ++sourceX) {
                int16_t destinationX = static_cast<int16_t>(sourceX + shift);
                if (destinationX >= PoomScreenWidth) {
                    destinationX = static_cast<int16_t>(destinationX - PoomScreenWidth);
                }
                shiftedPage[destinationX] = source[sourceX];
            }
            pageData = shiftedPage;
        }

        uint8_t *shadowPage = shadowBuffer_ + static_cast<size_t>(page) * PoomScreenWidth;
        if (!shadowValid_) {
            if (!sendPageRange(page, 0, pageData, PoomScreenWidth, dataBytesSent)) {
                shadowValid_ = false;
                return false;
            }
            memcpy(shadowPage, pageData, PoomScreenWidth);
            continue;
        }

        uint8_t column = 0;
        while (column < PoomScreenWidth) {
            while (column < PoomScreenWidth && pageData[column] == shadowPage[column]) {
                ++column;
            }
            if (column >= PoomScreenWidth) {
                break;
            }

            const uint8_t startColumn = column;
            uint8_t lastChangedColumn = column;
            ++column;

            while (column < PoomScreenWidth) {
                if (pageData[column] != shadowPage[column]) {
                    lastChangedColumn = column;
                }
                else if (column - lastChangedColumn > MergeGapBytes) {
                    break;
                }
                ++column;
            }

            const size_t length = static_cast<size_t>(lastChangedColumn - startColumn) + 1U;
            if (!sendPageRange(
                    page,
                    startColumn,
                    pageData + startColumn,
                    length,
                    dataBytesSent
                )) {
                shadowValid_ = false;
                return false;
            }
            memcpy(shadowPage + startColumn, pageData + startColumn, length);
        }
    }

    shadowValid_ = true;
    return true;
}

bool PoomDisplay::sendPageRange(
    uint8_t page,
    uint8_t startColumn,
    const uint8_t *data,
    size_t length,
    size_t &dataBytesSent
)
{
    constexpr uint8_t ColumnOffset = 2;
    const uint8_t displayColumn = static_cast<uint8_t>(ColumnOffset + startColumn);

    Wire.beginTransmission(POOM_DISPLAY_I2C_ADDR);
    Wire.write(0x00);
    Wire.write(static_cast<uint8_t>(displayColumn & 0x0F));
    Wire.write(static_cast<uint8_t>(0x10U | (displayColumn >> 4)));
    Wire.write(static_cast<uint8_t>(0xB0U | page));
    if (Wire.endTransmission() != 0) {
        return false;
    }

    for (size_t offset = 0; offset < length;) {
        const size_t remaining = length - offset;

        Wire.beginTransmission(POOM_DISPLAY_I2C_ADDR);
        if (Wire.write(0x40) != 1U) {
            return false;
        }

        if (i2cDataCapacity_ == 0) {
            return false;
        }
        const size_t chunk = remaining < i2cDataCapacity_
            ? remaining
            : i2cDataCapacity_;
        if (Wire.write(data + offset, chunk) != chunk || Wire.endTransmission() != 0) {
            return false;
        }
        dataBytesSent += chunk;
        offset += chunk;
    }

    return true;
}

void PoomDisplay::recordPresentation(
    size_t dataBytesSent,
    uint32_t elapsedMicros,
    bool error
)
{
    ++metrics_.presentCount;
    ++windowPresentCount_;
    metrics_.lastPresentMicros = elapsedMicros;
    if (elapsedMicros > metrics_.maxPresentMicros) {
        metrics_.maxPresentMicros = elapsedMicros;
    }
    metrics_.lastDataBytes = static_cast<uint16_t>(dataBytesSent);

    if (dataBytesSent == 0) {
        ++metrics_.skippedCount;
    }
    else {
        ++metrics_.transferCount;
        ++windowTransferCount_;
        metrics_.dataBytesSent += dataBytesSent;
        if (dataBytesSent >= PoomFramebufferSize) {
            ++metrics_.fullTransferCount;
        }
        else {
            ++metrics_.partialTransferCount;
        }
    }
    if (error) {
        ++metrics_.errorCount;
    }

    const uint32_t nowMs = millis();
    if (!metricsWindowStarted_) {
        metricsWindowStarted_ = true;
        metricsWindowStartMs_ = nowMs;
        windowPresentCount_ = 0;
        windowTransferCount_ = 0;
        return;
    }

    const uint32_t elapsedMs = nowMs - metricsWindowStartMs_;
    if (elapsedMs >= 1000U) {
        metrics_.presentFps = static_cast<uint16_t>(
            (static_cast<uint32_t>(windowPresentCount_) * 1000U + elapsedMs / 2U) /
            elapsedMs
        );
        metrics_.transferFps = static_cast<uint16_t>(
            (static_cast<uint32_t>(windowTransferCount_) * 1000U + elapsedMs / 2U) /
            elapsedMs
        );
        metricsWindowStartMs_ = nowMs;
        windowPresentCount_ = 0;
        windowTransferCount_ = 0;
    }
}

void PoomDisplay::drawBufferWithRotation(const uint8_t *buffer)
{
    oled_.clearDisplay();

    for (uint8_t y = 0; y < PoomScreenHeight; ++y) {
        const uint8_t row = y >> 3;
        const uint8_t bit = 1U << (y & 7);

        for (uint8_t x = 0; x < PoomScreenWidth; ++x) {
            const size_t index = static_cast<size_t>(row) * PoomScreenWidth + x;
            if ((buffer[index] & bit) == 0) {
                continue;
            }

            int16_t mappedX = static_cast<int16_t>(
                (static_cast<int32_t>(x) + POOM_DISPLAY_X_SHIFT) % PoomScreenWidth
            );
            if (mappedX < 0) {
                mappedX = static_cast<int16_t>(mappedX + PoomScreenWidth);
            }

            oled_.drawPixel(mappedX, y, SH110X_WHITE);
        }
    }
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
    // A caller can mutate or flush the Adafruit-owned framebuffer through this
    // reference, so the direct-transfer shadow can no longer be trusted.
    shadowValid_ = false;
    return oled_;
}

void PoomDisplay::applyTextDefaults()
{
    oled_.setTextSize(1);
    oled_.setTextColor(SH110X_WHITE);
    oled_.setCursor(0, 0);
}
