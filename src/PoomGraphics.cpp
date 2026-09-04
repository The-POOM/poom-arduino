#include "PoomGraphics.h"

void PoomGraphics::attach(PoomFramebuffer *framebuffer)
{
    framebuffer_ = framebuffer;
}

void PoomGraphics::drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t width, uint8_t height)
{
    drawBitmapInternal(x, y, bitmap, nullptr, width, height, false);
}

void PoomGraphics::drawBitmapMasked(
    int16_t x,
    int16_t y,
    const uint8_t *bitmap,
    const uint8_t *mask,
    uint8_t width,
    uint8_t height
)
{
    drawBitmapInternal(x, y, bitmap, mask, width, height, true);
}

void PoomGraphics::drawSprite(
    int16_t x,
    int16_t y,
    const uint8_t *frames,
    uint8_t width,
    uint8_t height,
    uint8_t frame
)
{
    if (!frames) {
        return;
    }

    drawBitmap(x, y, frames + (frameSize(width, height) * frame), width, height);
}

void PoomGraphics::drawSpriteMasked(
    int16_t x,
    int16_t y,
    const uint8_t *frames,
    const uint8_t *masks,
    uint8_t width,
    uint8_t height,
    uint8_t frame
)
{
    drawSpriteMaskedInternal(x, y, frames, masks, width, height, frame);
}

void PoomGraphics::drawPageBitmap(
    int16_t x,
    int16_t y,
    const uint8_t *bitmap,
    uint8_t width,
    uint8_t height
)
{
    drawPageBitmapInternal(x, y, bitmap, nullptr, width, height, false);
}

void PoomGraphics::drawPageBitmapMasked(
    int16_t x,
    int16_t y,
    const uint8_t *bitmap,
    const uint8_t *mask,
    uint8_t width,
    uint8_t height
)
{
    drawPageBitmapInternal(x, y, bitmap, mask, width, height, true);
}

void PoomGraphics::drawPageSprite(
    int16_t x,
    int16_t y,
    const uint8_t *frames,
    uint8_t width,
    uint8_t height,
    uint8_t frame
)
{
    if (!frames) {
        return;
    }

    drawPageBitmap(x, y, frames + (pageFrameSize(width, height) * frame), width, height);
}

size_t PoomGraphics::frameSize(uint8_t width, uint8_t height) const
{
    return ((static_cast<size_t>(width) + 7U) / 8U) * static_cast<size_t>(height);
}

bool PoomGraphics::isReady() const
{
    return framebuffer_ != nullptr;
}

bool PoomGraphics::sampleBit(const uint8_t *bitmap, uint8_t width, int16_t x, int16_t y) const
{
    if (!bitmap || x < 0 || y < 0 || x >= width) {
        return false;
    }

    const size_t rowBytes = (static_cast<size_t>(width) + 7U) / 8U;
    const size_t index = (static_cast<size_t>(y) * rowBytes) + (static_cast<size_t>(x) / 8U);
    const uint8_t mask = static_cast<uint8_t>(0x80U >> (x & 7));
    return (pgm_read_byte(bitmap + index) & mask) != 0;
}

bool PoomGraphics::samplePageBit(const uint8_t *bitmap, uint8_t width, int16_t x, int16_t y) const
{
    if (!bitmap || x < 0 || y < 0 || x >= width) {
        return false;
    }

    const size_t index = (static_cast<size_t>(y) / 8U) * width + static_cast<size_t>(x);
    const uint8_t mask = static_cast<uint8_t>(1U << (y & 7));
    return (pgm_read_byte(bitmap + index) & mask) != 0;
}

size_t PoomGraphics::pageFrameSize(uint8_t width, uint8_t height) const
{
    return static_cast<size_t>(width) * ((static_cast<size_t>(height) + 7U) / 8U);
}

void PoomGraphics::drawBitmapInternal(
    int16_t x,
    int16_t y,
    const uint8_t *bitmap,
    const uint8_t *mask,
    uint8_t width,
    uint8_t height,
    bool overwrite
)
{
    if (!isReady() || !bitmap || width == 0 || height == 0) {
        return;
    }

    const int32_t destinationLeft = x;
    const int32_t destinationTop = y;
    const int32_t destinationRight = destinationLeft + width;
    const int32_t destinationBottom = destinationTop + height;

    if (destinationRight <= 0 || destinationBottom <= 0 ||
        destinationLeft >= PoomScreenWidth || destinationTop >= PoomScreenHeight) {
        return;
    }

    int16_t startX = destinationLeft < 0 ? static_cast<int16_t>(-destinationLeft) : 0;
    int16_t startY = destinationTop < 0 ? static_cast<int16_t>(-destinationTop) : 0;
    int16_t endX = width;
    int16_t endY = height;

    if (destinationRight > PoomScreenWidth) {
        endX = static_cast<int16_t>(PoomScreenWidth - destinationLeft);
    }
    if (destinationBottom > PoomScreenHeight) {
        endY = static_cast<int16_t>(PoomScreenHeight - destinationTop);
    }

    if (startX >= endX || startY >= endY) {
        return;
    }

    for (int16_t sy = startY; sy < endY; ++sy) {
        for (int16_t sx = startX; sx < endX; ++sx) {
            if (mask && !sampleBit(mask, width, sx, sy)) {
                continue;
            }

            const bool pixelOn = sampleBit(bitmap, width, sx, sy);
            if (!overwrite && !pixelOn) {
                continue;
            }

            framebuffer_->drawPixel(x + sx, y + sy, pixelOn);
        }
    }
}

void PoomGraphics::drawPageBitmapInternal(
    int16_t x,
    int16_t y,
    const uint8_t *bitmap,
    const uint8_t *mask,
    uint8_t width,
    uint8_t height,
    bool overwrite
)
{
    if (!isReady() || !bitmap || width == 0 || height == 0) {
        return;
    }

    const int32_t destinationLeft = x;
    const int32_t destinationTop = y;
    const int32_t destinationRight = destinationLeft + width;
    const int32_t destinationBottom = destinationTop + height;

    if (destinationRight <= 0 || destinationBottom <= 0 ||
        destinationLeft >= PoomScreenWidth || destinationTop >= PoomScreenHeight) {
        return;
    }

    int16_t startX = destinationLeft < 0 ? static_cast<int16_t>(-destinationLeft) : 0;
    int16_t startY = destinationTop < 0 ? static_cast<int16_t>(-destinationTop) : 0;
    int16_t endX = width;
    int16_t endY = height;

    if (destinationRight > PoomScreenWidth) {
        endX = static_cast<int16_t>(PoomScreenWidth - destinationLeft);
    }
    if (destinationBottom > PoomScreenHeight) {
        endY = static_cast<int16_t>(PoomScreenHeight - destinationTop);
    }

    if (startX >= endX || startY >= endY) {
        return;
    }

    for (int16_t sy = startY; sy < endY; ++sy) {
        for (int16_t sx = startX; sx < endX; ++sx) {
            if (mask && !samplePageBit(mask, width, sx, sy)) {
                continue;
            }

            const bool pixelOn = samplePageBit(bitmap, width, sx, sy);
            if (!overwrite && !pixelOn) {
                continue;
            }

            framebuffer_->drawPixel(x + sx, y + sy, pixelOn);
        }
    }
}

void PoomGraphics::drawSpriteMaskedInternal(
    int16_t x,
    int16_t y,
    const uint8_t *frames,
    const uint8_t *masks,
    uint8_t width,
    uint8_t height,
    uint8_t frame
)
{
    if (!frames || !masks) {
        return;
    }

    const size_t offset = frameSize(width, height) * frame;
    drawBitmapMasked(x, y, frames + offset, masks + offset, width, height);
}
