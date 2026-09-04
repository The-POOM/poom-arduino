#include "PoomGraphics.h"

namespace
{
class PoomBitStreamReader
{
public:
    explicit PoomBitStreamReader(const uint8_t *source)
        : source_(source)
    {
    }

    uint32_t readBits(uint8_t bitCount)
    {
        uint32_t result = 0;
        for (uint8_t bitIndex = 0; bitIndex < bitCount; ++bitIndex) {
            if (bitMask_ == 0) {
                bitMask_ = 0x01;
                byte_ = pgm_read_byte(source_ + sourceIndex_);
                ++sourceIndex_;
            }

            if ((byte_ & bitMask_) != 0) {
                result |= static_cast<uint32_t>(1UL << bitIndex);
            }
            bitMask_ = static_cast<uint8_t>(bitMask_ << 1);
        }
        return result;
    }

private:
    const uint8_t *source_;
    size_t sourceIndex_ = 0;
    uint8_t bitMask_ = 0;
    uint8_t byte_ = 0;
};
}

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
    drawPageBitmapInternal(x, y, bitmap, nullptr, width, height, 0, 0, false, PageBlitMode::Set);
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
    drawPageBitmapInternal(x, y, bitmap, mask, width, height, 0, 0, false, PageBlitMode::Masked);
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
    drawPageBitmapInternal(x, y, frames, nullptr, width, height, frame, frame, false, PageBlitMode::Set);
}

void PoomGraphics::drawPageSpriteMasked(
    int16_t x,
    int16_t y,
    const uint8_t *frames,
    const uint8_t *masks,
    uint8_t width,
    uint8_t height,
    uint8_t frame
)
{
    drawPageSpriteMasked(x, y, frames, masks, width, height, frame, frame);
}

void PoomGraphics::drawPageSpriteMasked(
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
    drawPageBitmapInternal(
        x, y, frames, masks, width, height,
        imageFrame, maskFrame, false, PageBlitMode::Masked
    );
}

void PoomGraphics::drawPageSpriteInterleavedMasked(
    int16_t x,
    int16_t y,
    const uint8_t *frames,
    uint8_t width,
    uint8_t height,
    uint8_t frame
)
{
    drawPageBitmapInternal(x, y, frames, nullptr, width, height, frame, frame, true, PageBlitMode::Masked);
}

void PoomGraphics::erasePageSprite(
    int16_t x,
    int16_t y,
    const uint8_t *frames,
    uint8_t width,
    uint8_t height,
    uint8_t frame
)
{
    drawPageBitmapInternal(x, y, frames, nullptr, width, height, frame, frame, false, PageBlitMode::Erase);
}

void PoomGraphics::overwritePageSprite(
    int16_t x,
    int16_t y,
    const uint8_t *frames,
    uint8_t width,
    uint8_t height,
    uint8_t frame
)
{
    drawPageBitmapInternal(x, y, frames, nullptr, width, height, frame, frame, false, PageBlitMode::Overwrite);
}

void PoomGraphics::drawPageAsset(int16_t x, int16_t y, const uint8_t *asset, uint8_t frame)
{
    if (!asset) {
        return;
    }

    const uint8_t width = pgm_read_byte(asset);
    const uint8_t height = pgm_read_byte(asset + 1);
    drawPageSprite(x, y, asset + 2, width, height, frame);
}

void PoomGraphics::drawPageAssetMasked(
    int16_t x,
    int16_t y,
    const uint8_t *asset,
    const uint8_t *mask,
    uint8_t imageFrame,
    uint8_t maskFrame
)
{
    if (!asset || !mask) {
        return;
    }

    const uint8_t width = pgm_read_byte(asset);
    const uint8_t height = pgm_read_byte(asset + 1);
    if (width != pgm_read_byte(mask) || height != pgm_read_byte(mask + 1)) {
        return;
    }

    drawPageSpriteMasked(
        x, y, asset + 2, mask + 2, width, height, imageFrame, maskFrame
    );
}

void PoomGraphics::drawPageAssetInterleavedMasked(
    int16_t x,
    int16_t y,
    const uint8_t *asset,
    uint8_t frame
)
{
    if (!asset) {
        return;
    }

    const uint8_t width = pgm_read_byte(asset);
    const uint8_t height = pgm_read_byte(asset + 1);
    drawPageSpriteInterleavedMasked(x, y, asset + 2, width, height, frame);
}

void PoomGraphics::erasePageAsset(int16_t x, int16_t y, const uint8_t *asset, uint8_t frame)
{
    if (!asset) {
        return;
    }

    const uint8_t width = pgm_read_byte(asset);
    const uint8_t height = pgm_read_byte(asset + 1);
    erasePageSprite(x, y, asset + 2, width, height, frame);
}

void PoomGraphics::overwritePageAsset(int16_t x, int16_t y, const uint8_t *asset, uint8_t frame)
{
    if (!asset) {
        return;
    }

    const uint8_t width = pgm_read_byte(asset);
    const uint8_t height = pgm_read_byte(asset + 1);
    overwritePageSprite(x, y, asset + 2, width, height, frame);
}

void PoomGraphics::drawCompressed(int16_t x, int16_t y, const uint8_t *bitmap, bool on)
{
    drawCompressedMirror(x, y, bitmap, on, false);
}

void PoomGraphics::drawCompressedMirror(
    int16_t x,
    int16_t y,
    const uint8_t *bitmap,
    bool on,
    bool mirror
)
{
    if (!isReady() || !bitmap) {
        return;
    }

    PoomBitStreamReader stream(bitmap);
    const int16_t width = static_cast<int16_t>(stream.readBits(8) + 1U);
    const int16_t height = static_cast<int16_t>(stream.readBits(8) + 1U);
    uint8_t spanOn = static_cast<uint8_t>(stream.readBits(1));

    const int32_t right = static_cast<int32_t>(x) + width;
    const int32_t bottom = static_cast<int32_t>(y) + height;
    if (right <= 0 || bottom <= 0 || x >= PoomScreenWidth || y >= PoomScreenHeight) {
        return;
    }

    int16_t startPage = static_cast<int16_t>(y / 8);
    int16_t remainder = static_cast<int16_t>(y % 8);
    if (remainder < 0) {
        remainder = static_cast<int16_t>(remainder + 8);
        --startPage;
    }
    const uint8_t shift = static_cast<uint8_t>(remainder);
    const int16_t pageCount = static_cast<int16_t>((height + 7) / 8);
    int16_t sourcePage = 0;
    int16_t sourceX = mirror ? static_cast<int16_t>(width - 1) : 0;
    uint8_t decodedByte = 0;
    uint8_t decodedBit = 0x01;
    uint8_t *destination = framebuffer_->data();

    while (sourcePage < pageCount) {
        uint8_t bitLength = 1;
        while (stream.readBits(1) == 0) {
            bitLength = static_cast<uint8_t>(bitLength + 2);
        }

        const uint32_t spanLength = stream.readBits(bitLength) + 1U;
        for (uint32_t spanIndex = 0; spanIndex < spanLength; ++spanIndex) {
            if (spanOn != 0) {
                decodedByte |= decodedBit;
            }
            decodedBit = static_cast<uint8_t>(decodedBit << 1);

            if (decodedBit != 0) {
                continue;
            }

            const int16_t destinationX = static_cast<int16_t>(x + sourceX);
            const int16_t destinationPage = static_cast<int16_t>(startPage + sourcePage);
            if (destinationX >= 0 && destinationX < PoomScreenWidth) {
                if (destinationPage >= 0 && destinationPage < PoomScreenHeight / 8) {
                    const size_t index = static_cast<size_t>(destinationPage) * PoomScreenWidth + destinationX;
                    const uint8_t value = static_cast<uint8_t>(decodedByte << shift);
                    if (on) {
                        destination[index] |= value;
                    } else {
                        destination[index] &= static_cast<uint8_t>(~value);
                    }
                }

                if (shift != 0 && destinationPage + 1 >= 0 &&
                    destinationPage + 1 < PoomScreenHeight / 8) {
                    const size_t index = static_cast<size_t>(destinationPage + 1) *
                        PoomScreenWidth + destinationX;
                    const uint8_t value = static_cast<uint8_t>(decodedByte >> (8U - shift));
                    if (on) {
                        destination[index] |= value;
                    } else {
                        destination[index] &= static_cast<uint8_t>(~value);
                    }
                }
            }

            if (mirror) {
                --sourceX;
                if (sourceX < 0) {
                    sourceX = static_cast<int16_t>(width - 1);
                    ++sourcePage;
                }
            } else {
                ++sourceX;
                if (sourceX >= width) {
                    sourceX = 0;
                    ++sourcePage;
                }
            }

            decodedByte = 0;
            decodedBit = 0x01;
        }
        spanOn ^= 0x01U;
    }
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
    uint8_t imageFrameIndex,
    uint8_t maskFrameIndex,
    bool interleaved,
    PageBlitMode mode
)
{
    if (!isReady() || !bitmap || width == 0 || height == 0) {
        return;
    }

    if (mode == PageBlitMode::Masked && !interleaved && !mask) {
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
    int16_t endX = width;

    if (destinationRight > PoomScreenWidth) {
        endX = static_cast<int16_t>(PoomScreenWidth - destinationLeft);
    }

    if (startX >= endX) {
        return;
    }

    const size_t bytesPerFrame = pageFrameSize(width, height);
    const size_t frameOffset = bytesPerFrame * imageFrameIndex * (interleaved ? 2U : 1U);
    const uint8_t *imageFrame = bitmap + frameOffset;
    const uint8_t *maskFrame = mask ? mask + (bytesPerFrame * maskFrameIndex) : nullptr;
    const uint8_t sourcePages = static_cast<uint8_t>((height + 7U) / 8U);
    const uint8_t finalPageBits = static_cast<uint8_t>(height & 7U);
    uint8_t *destination = framebuffer_->data();

    for (uint8_t sourcePage = 0; sourcePage < sourcePages; ++sourcePage) {
        const int16_t sourceTop = static_cast<int16_t>(y + static_cast<int16_t>(sourcePage * 8U));
        const int16_t destinationPage = sourceTop >= 0
            ? static_cast<int16_t>(sourceTop / 8)
            : static_cast<int16_t>(-((7 - sourceTop) / 8));
        const uint8_t shift = static_cast<uint8_t>(sourceTop - destinationPage * 8);
        const uint8_t validBits = (sourcePage + 1U == sourcePages && finalPageBits != 0)
            ? static_cast<uint8_t>((1U << finalPageBits) - 1U)
            : 0xFFU;

        for (int16_t sourceX = startX; sourceX < endX; ++sourceX) {
            const size_t sourceIndex = static_cast<size_t>(sourcePage) * width + sourceX;
            uint8_t image;
            uint8_t coverage;

            if (interleaved) {
                image = pgm_read_byte(imageFrame + sourceIndex * 2U);
                coverage = pgm_read_byte(imageFrame + sourceIndex * 2U + 1U);
            } else {
                image = pgm_read_byte(imageFrame + sourceIndex);
                coverage = maskFrame ? pgm_read_byte(maskFrame + sourceIndex) : validBits;
            }

            image &= validBits;
            coverage &= validBits;

            if (mode == PageBlitMode::Set || mode == PageBlitMode::Erase) {
                coverage = image;
            }
            if (mode == PageBlitMode::Erase) {
                image = 0;
            }

            const int16_t destinationX = static_cast<int16_t>(x + sourceX);
            if (destinationPage >= 0 && destinationPage < PoomScreenHeight / 8) {
                const uint8_t pageImage = static_cast<uint8_t>(image << shift);
                const uint8_t pageCoverage = static_cast<uint8_t>(coverage << shift);
                const size_t destinationIndex =
                    static_cast<size_t>(destinationPage) * PoomScreenWidth + destinationX;
                destination[destinationIndex] = static_cast<uint8_t>(
                    (destination[destinationIndex] & static_cast<uint8_t>(~pageCoverage)) |
                    (pageImage & pageCoverage)
                );
            }

            if (shift != 0 && destinationPage + 1 >= 0 && destinationPage + 1 < PoomScreenHeight / 8) {
                const uint8_t pageImage = static_cast<uint8_t>(image >> (8U - shift));
                const uint8_t pageCoverage = static_cast<uint8_t>(coverage >> (8U - shift));
                const size_t destinationIndex =
                    static_cast<size_t>(destinationPage + 1) * PoomScreenWidth + destinationX;
                destination[destinationIndex] = static_cast<uint8_t>(
                    (destination[destinationIndex] & static_cast<uint8_t>(~pageCoverage)) |
                    (pageImage & pageCoverage)
                );
            }
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
