#include "PoomFX.h"

#include "PoomPrinceAssets.h"

namespace
{
constexpr const char *SaveKey = "save";
constexpr uint16_t SaveVersion = 1;
}

uint32_t FX::readCursor_ = 0;
FrameControl FX::frameControl_ = {0, 0, 0, 0};
uint16_t FX::presentationAccumulator_ = 0;
uint16_t FX::slowPresentationTicks_ = 0;
uint8_t FX::updateRate_ = 60;
uint8_t FX::fallbackPresentationRate_ = 60;

void FX::begin()
{
}

void FX::begin(uint16_t)
{
    begin();
}

void FX::begin(uint16_t, uint16_t)
{
    begin();
}

void FX::display()
{
    presentFramebuffer();
}

void FX::display(bool clearBuffer)
{
    presentFramebuffer();
    if (clearBuffer) {
        Poom.clear();
    }
}

void FX::configurePresentationTiming(uint8_t updateRate, uint8_t fallbackRate)
{
    updateRate_ = updateRate == 0 ? 1 : updateRate;
    fallbackPresentationRate_ = fallbackRate == 0 ? 1 : fallbackRate;
    if (fallbackPresentationRate_ > updateRate_) {
        fallbackPresentationRate_ = updateRate_;
    }
    presentationAccumulator_ = 0;
    slowPresentationTicks_ = 0;
}

void FX::presentFramebuffer()
{
    const uint8_t currentUpdateRate = Poom.targetFrameRate();
    if (currentUpdateRate != updateRate_) {
        updateRate_ = currentUpdateRate;
        presentationAccumulator_ = 0;
        slowPresentationTicks_ = 0;
    }

    const bool usingFallbackRate = slowPresentationTicks_ != 0;
    if (usingFallbackRate) {
        --slowPresentationTicks_;
    }

    const uint8_t presentationRate = usingFallbackRate
        ? fallbackPresentationRate_
        : updateRate_;
    presentationAccumulator_ = static_cast<uint16_t>(
        presentationAccumulator_ + presentationRate
    );
    if (presentationAccumulator_ < updateRate_) {
        return;
    }
    presentationAccumulator_ = static_cast<uint16_t>(
        presentationAccumulator_ - updateRate_
    );

    Poom.show();
    const PoomDisplayMetrics &metrics = Poom.screen().metrics();
    const uint32_t updateBudgetUs = 1000000UL / updateRate_;
    if (metrics.lastPresentMicros > updateBudgetUs) {
        slowPresentationTicks_ = updateRate_;
    }
}

void FX::seekData(uint24_t address)
{
    readCursor_ = static_cast<uint32_t>(address);
}

void FX::seekDataArray(
    uint24_t address,
    uint8_t index,
    uint8_t offset,
    uint8_t elementSize
)
{
    uint32_t absolute = static_cast<uint32_t>(address) + offset;
    absolute += elementSize == 0
        ? static_cast<uint32_t>(index) * 256U
        : static_cast<uint32_t>(index) * elementSize;
    seekData(static_cast<uint24_t>(absolute));
}

uint8_t FX::readPendingUInt8()
{
    return readDataByte(readCursor_++);
}

uint16_t FX::readPendingUInt16()
{
    return readUInt16();
}

uint16_t FX::readPendingLastUInt16()
{
    return readUInt16();
}

uint32_t FX::readPendingLastUInt32()
{
    uint32_t value = static_cast<uint32_t>(readPendingUInt8()) << 24;
    value |= static_cast<uint32_t>(readPendingUInt8()) << 16;
    value |= static_cast<uint32_t>(readPendingUInt8()) << 8;
    value |= readPendingUInt8();
    return value;
}

uint8_t FX::readEnd()
{
    return readPendingUInt8();
}

void FX::readBytes(uint8_t *buffer, size_t length)
{
    if (!buffer) {
        return;
    }
    for (size_t index = 0; index < length; ++index) {
        buffer[index] = readPendingUInt8();
    }
}

uint24_t FX::readIndexedUInt24(uint24_t address, uint8_t index)
{
    seekDataArray(address, index, 0, 3);
    return readUInt24();
}

void FX::saveGameState(const uint8_t *gameState, size_t size)
{
    if (!gameState || size == 0) {
        return;
    }
    Poom.storage().writeRecord(SaveKey, SaveVersion, gameState, size);
}

uint8_t FX::loadGameState(uint8_t *gameState, size_t size)
{
    if (!gameState || size == 0) {
        return 0;
    }
    return Poom.storage().readRecord(SaveKey, SaveVersion, gameState, size) ? 1 : 0;
}

void FX::drawBitmap(
    int16_t x,
    int16_t y,
    uint24_t address,
    uint8_t frame,
    uint8_t mode
)
{
    const uint32_t assetAddress = static_cast<uint32_t>(address);
    seekData(address);
    const int16_t width = static_cast<int16_t>(readUInt16());
    const int16_t height = static_cast<int16_t>(readUInt16());
    if (width <= 0 || height <= 0) {
        return;
    }

    const int32_t right = static_cast<int32_t>(x) + width;
    const int32_t bottom = static_cast<int32_t>(y) + height;
    if (right <= 0 || bottom <= 0 || x >= WIDTH || y >= HEIGHT) {
        return;
    }

    const bool masked = (mode & (1U << dbfMasked)) != 0;
    const bool flipped = (mode & (1U << dbfFlip)) != 0;
    const uint32_t bytesPerColumn = masked ? 2U : 1U;
    const uint16_t sourcePages = static_cast<uint16_t>((height + 7) / 8);
    const uint32_t bytesPerFrame = static_cast<uint32_t>(width) *
        sourcePages * bytesPerColumn;
    const uint32_t frameAddress = assetAddress + 4U +
        static_cast<uint32_t>(frame) * bytesPerFrame;
    uint8_t *framebuffer = Poom.buffer();

    for (uint16_t sourcePage = 0; sourcePage < sourcePages; ++sourcePage) {
        uint8_t validMask = 0xFF;
        if (sourcePage + 1U == sourcePages && (height & 7) != 0) {
            validMask = static_cast<uint8_t>((1U << (height & 7)) - 1U);
        }

        const int16_t pageY = static_cast<int16_t>(
            y + static_cast<int16_t>(sourcePage * 8U)
        );
        int16_t destinationPage = static_cast<int16_t>(pageY / 8);
        int16_t shift = static_cast<int16_t>(pageY % 8);
        if (shift < 0) {
            shift = static_cast<int16_t>(shift + 8);
            --destinationPage;
        }

        for (int16_t sourceX = 0; sourceX < width; ++sourceX) {
            const int16_t destinationX = flipped
                ? static_cast<int16_t>(x + width - 1 - sourceX)
                : static_cast<int16_t>(x + sourceX);
            if (destinationX < 0 || destinationX >= WIDTH) {
                continue;
            }

            const uint32_t sourceAddress = frameAddress +
                (static_cast<uint32_t>(sourcePage) * width + sourceX) *
                bytesPerColumn;
            uint8_t imageByte = static_cast<uint8_t>(
                readDataByte(sourceAddress) & validMask
            );
            if ((mode & (1U << dbfReverseBlack)) != 0) {
                imageByte ^= validMask;
            }

            uint8_t maskByte = masked
                ? static_cast<uint8_t>(readDataByte(sourceAddress + 1U) & validMask)
                : validMask;
            if ((mode & (1U << dbfWhiteBlack)) != 0) {
                maskByte = imageByte;
            }
            if ((mode & (1U << dbfBlack)) != 0) {
                imageByte = 0;
            }

            const uint16_t image = static_cast<uint16_t>(imageByte) << shift;
            const uint16_t mask = static_cast<uint16_t>(maskByte) << shift;
            const bool invert = (mode & (1U << dbfInvert)) != 0;

            if (destinationPage >= 0 && destinationPage < HEIGHT / 8) {
                const size_t index = static_cast<size_t>(destinationPage) * WIDTH +
                    destinationX;
                const uint8_t pixels = static_cast<uint8_t>(image);
                const uint8_t pageMask = static_cast<uint8_t>(mask);
                if (invert) {
                    framebuffer[index] ^= static_cast<uint8_t>(pixels & pageMask);
                } else {
                    framebuffer[index] = static_cast<uint8_t>(
                        (framebuffer[index] & static_cast<uint8_t>(~pageMask)) |
                        (pixels & pageMask)
                    );
                }
            }

            const int16_t lowerPage = static_cast<int16_t>(destinationPage + 1);
            if (shift != 0 && lowerPage >= 0 && lowerPage < HEIGHT / 8) {
                const size_t index = static_cast<size_t>(lowerPage) * WIDTH +
                    destinationX;
                const uint8_t pixels = static_cast<uint8_t>(image >> 8);
                const uint8_t pageMask = static_cast<uint8_t>(mask >> 8);
                if (invert) {
                    framebuffer[index] ^= static_cast<uint8_t>(pixels & pageMask);
                } else {
                    framebuffer[index] = static_cast<uint8_t>(
                        (framebuffer[index] & static_cast<uint8_t>(~pageMask)) |
                        (pixels & pageMask)
                    );
                }
            }
        }
    }
}

void FX::setFrame(uint24_t frame, uint8_t repeat)
{
    frameControl_.start = frame;
    frameControl_.current = frame;
    frameControl_.repeat = repeat;
    frameControl_.count = repeat;
}

uint8_t FX::drawFrame()
{
    uint24_t frame = drawFrame(frameControl_.current);
    const uint8_t moreFrames = static_cast<uint8_t>((frame != 0) || frameControl_.count != 0);

    if (frameControl_.count > 0) {
        --frameControl_.count;
    } else {
        frameControl_.count = frameControl_.repeat;
        if (!moreFrames) {
            frame = frameControl_.start;
        }
        frameControl_.current = frame;
    }
    return moreFrames;
}

uint24_t FX::drawFrame(uint24_t address)
{
    constexpr uint8_t FrameEntrySize = 9;
    for (;;) {
        seekData(address);
        const int16_t x = static_cast<int16_t>(readUInt16());
        const int16_t y = static_cast<int16_t>(readUInt16());
        const uint24_t bitmap = readUInt24();
        const uint8_t frame = readPendingUInt8();
        const uint8_t mode = readEnd();
        drawBitmap(x, y, bitmap, frame, mode);

        address = static_cast<uint24_t>(static_cast<uint32_t>(address) + FrameEntrySize);
        if ((mode & dbmEndFrame) != 0) {
            return address;
        }
        if ((mode & dbmLastFrame) != 0) {
            return 0;
        }
    }
}

uint8_t FX::readDataByte(uint32_t address)
{
    if (address >= PoomPrinceAssetsLength) {
        return 0xFF;
    }
    return pgm_read_byte(PoomPrinceAssets + address);
}

uint16_t FX::readUInt16()
{
    const uint16_t high = readPendingUInt8();
    return static_cast<uint16_t>((high << 8) | readPendingUInt8());
}

uint24_t FX::readUInt24()
{
    uint32_t value = static_cast<uint32_t>(readPendingUInt8()) << 16;
    value |= static_cast<uint32_t>(readPendingUInt8()) << 8;
    value |= readPendingUInt8();
    return static_cast<uint24_t>(value);
}
