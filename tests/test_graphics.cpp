#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

#include "PoomGraphics.h"

namespace
{
enum class Mode
{
    Set,
    Masked,
    Erase,
    Overwrite
};

using Buffer = std::array<uint8_t, PoomFramebufferSize>;

constexpr uint8_t Width = 7;
constexpr uint8_t Height = 13;
constexpr uint8_t FrameCount = 3;
constexpr size_t FrameSize = Width * ((Height + 7U) / 8U);

const std::array<uint8_t, FrameSize * FrameCount> Images = {
    0x81, 0x42, 0x24, 0x18, 0x3C, 0x66, 0xFF,
    0x15, 0x0A, 0x11, 0x04, 0x1F, 0x12, 0x09,
    0x7E, 0x3C, 0x18, 0x00, 0x5A, 0x24, 0x42,
    0x01, 0x02, 0x04, 0x08, 0x10, 0x08, 0x04,
    0xC3, 0xA5, 0x99, 0x81, 0xBD, 0xA5, 0xC3,
    0x1C, 0x0E, 0x07, 0x03, 0x01, 0x10, 0x08
};

const std::array<uint8_t, FrameSize * FrameCount> Masks = {
    0xFF, 0x7E, 0x3C, 0x18, 0x3C, 0x7E, 0xFF,
    0x1F, 0x1E, 0x1C, 0x18, 0x1C, 0x1E, 0x1F,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA,
    0x1F, 0x0F, 0x07, 0x03, 0x07, 0x0F, 0x1F,
    0xF0, 0x0F, 0xCC, 0x33, 0xAA, 0x55, 0xFF,
    0x11, 0x12, 0x14, 0x18, 0x1C, 0x1E, 0x1F
};

Buffer initialBuffer()
{
    Buffer buffer{};
    for (size_t index = 0; index < buffer.size(); ++index) {
        buffer[index] = static_cast<uint8_t>(0xA5U ^ (index * 37U));
    }
    return buffer;
}

void setPixel(Buffer &buffer, int16_t x, int16_t y, bool on)
{
    const size_t index = static_cast<size_t>(y / 8) * PoomScreenWidth + x;
    const uint8_t bit = static_cast<uint8_t>(1U << (y & 7));
    if (on) {
        buffer[index] |= bit;
    } else {
        buffer[index] &= static_cast<uint8_t>(~bit);
    }
}

bool sourcePixel(
    const uint8_t *data,
    uint8_t width,
    uint8_t height,
    uint8_t frame,
    bool interleaved,
    int16_t x,
    int16_t y,
    bool maskByte
)
{
    const size_t frameSize = static_cast<size_t>(width) * ((height + 7U) / 8U);
    const size_t pixelByte = static_cast<size_t>(y / 8) * width + x;
    const size_t frameOffset = static_cast<size_t>(frame) * frameSize * (interleaved ? 2U : 1U);
    const size_t index = frameOffset + pixelByte * (interleaved ? 2U : 1U) + (maskByte ? 1U : 0U);
    return (data[index] & static_cast<uint8_t>(1U << (y & 7))) != 0;
}

void referenceBlit(
    Buffer &destination,
    int16_t x,
    int16_t y,
    const uint8_t *images,
    const uint8_t *masks,
    uint8_t width,
    uint8_t height,
    uint8_t frame,
    bool interleaved,
    Mode mode,
    uint8_t maskFrame = 0xFF
)
{
    const uint8_t selectedMaskFrame = maskFrame == 0xFF ? frame : maskFrame;
    for (int16_t sourceY = 0; sourceY < height; ++sourceY) {
        const int16_t destinationY = static_cast<int16_t>(y + sourceY);
        if (destinationY < 0 || destinationY >= PoomScreenHeight) {
            continue;
        }

        for (int16_t sourceX = 0; sourceX < width; ++sourceX) {
            const int16_t destinationX = static_cast<int16_t>(x + sourceX);
            if (destinationX < 0 || destinationX >= PoomScreenWidth) {
                continue;
            }

            const bool image = sourcePixel(images, width, height, frame, interleaved, sourceX, sourceY, false);
            bool covered = false;
            switch (mode) {
            case Mode::Set:
            case Mode::Erase:
                covered = image;
                break;
            case Mode::Masked:
                covered = interleaved
                    ? sourcePixel(images, width, height, frame, true, sourceX, sourceY, true)
                    : sourcePixel(masks, width, height, selectedMaskFrame, false, sourceX, sourceY, false);
                break;
            case Mode::Overwrite:
                covered = true;
                break;
            }

            if (covered) {
                setPixel(destination, destinationX, destinationY, mode == Mode::Erase ? false : image);
            }
        }
    }

}

void expectEqual(const PoomFramebuffer &framebuffer, const Buffer &expected, const char *label)
{
    if (std::memcmp(framebuffer.data(), expected.data(), expected.size()) == 0) {
        return;
    }

    for (size_t index = 0; index < expected.size(); ++index) {
        if (framebuffer.data()[index] != expected[index]) {
            std::cerr << label << " failed at byte " << index << ": expected "
                      << static_cast<unsigned>(expected[index]) << ", got "
                      << static_cast<unsigned>(framebuffer.data()[index]) << '\n';
            std::exit(1);
        }
    }
}

std::vector<uint8_t> interleavedFrames()
{
    std::vector<uint8_t> result(Images.size() * 2U);
    for (size_t index = 0; index < Images.size(); ++index) {
        result[index * 2U] = Images[index];
        result[index * 2U + 1U] = Masks[index];
    }
    return result;
}

class BitWriter
{
public:
    void write(uint32_t value, uint8_t bitCount)
    {
        for (uint8_t bit = 0; bit < bitCount; ++bit) {
            if (bitIndex_ == 0) {
                bytes_.push_back(0);
            }
            if ((value & (1UL << bit)) != 0) {
                bytes_.back() |= static_cast<uint8_t>(1U << bitIndex_);
            }
            bitIndex_ = static_cast<uint8_t>((bitIndex_ + 1U) & 7U);
        }
    }

    const std::vector<uint8_t> &bytes() const
    {
        return bytes_;
    }

private:
    std::vector<uint8_t> bytes_;
    uint8_t bitIndex_ = 0;
};

std::vector<uint8_t> compressPageBitmap(
    const uint8_t *bitmap,
    uint8_t width,
    uint8_t height
)
{
    BitWriter writer;
    writer.write(width - 1U, 8);
    writer.write(height - 1U, 8);

    const size_t byteCount = static_cast<size_t>(width) * ((height + 7U) / 8U);
    const bool firstPixel = (bitmap[0] & 0x01U) != 0;
    writer.write(firstPixel ? 1U : 0U, 1);

    bool spanOn = firstPixel;
    size_t pixelIndex = 0;
    const size_t pixelCount = byteCount * 8U;
    while (pixelIndex < pixelCount) {
        size_t runLength = 0;
        while (pixelIndex + runLength < pixelCount) {
            const size_t current = pixelIndex + runLength;
            const bool pixelOn =
                (bitmap[current / 8U] & static_cast<uint8_t>(1U << (current & 7U))) != 0;
            if (pixelOn != spanOn) {
                break;
            }
            ++runLength;
        }

        uint8_t bitLength = 1;
        while (runLength - 1U >= (1UL << bitLength)) {
            bitLength = static_cast<uint8_t>(bitLength + 2U);
        }
        writer.write(0, static_cast<uint8_t>((bitLength - 1U) / 2U));
        writer.write(1, 1);
        writer.write(static_cast<uint32_t>(runLength - 1U), bitLength);

        pixelIndex += runLength;
        spanOn = !spanOn;
    }

    return writer.bytes();
}

void reset(PoomFramebuffer &framebuffer, Buffer &expected)
{
    expected = initialBuffer();
    std::memcpy(framebuffer.data(), expected.data(), expected.size());
}

void testPageModesAndClipping()
{
    PoomFramebuffer framebuffer;
    PoomGraphics graphics;
    graphics.attach(&framebuffer);
    Buffer expected{};
    const std::vector<uint8_t> interleaved = interleavedFrames();
    const std::array<int16_t, 7> yPositions = {-7, -1, 0, 1, 7, 8, 63};

    for (uint8_t frame = 0; frame < FrameCount; ++frame) {
        for (const int16_t y : yPositions) {
            reset(framebuffer, expected);
            graphics.drawPageSprite(9, y, Images.data(), Width, Height, frame);
            referenceBlit(expected, 9, y, Images.data(), nullptr, Width, Height, frame, false, Mode::Set);
            expectEqual(framebuffer, expected, "set/y/frame");

            reset(framebuffer, expected);
            graphics.drawPageSpriteMasked(9, y, Images.data(), Masks.data(), Width, Height, frame);
            referenceBlit(expected, 9, y, Images.data(), Masks.data(), Width, Height, frame, false, Mode::Masked);
            expectEqual(framebuffer, expected, "separate-mask/y/frame");

            reset(framebuffer, expected);
            graphics.drawPageSpriteInterleavedMasked(9, y, interleaved.data(), Width, Height, frame);
            referenceBlit(expected, 9, y, interleaved.data(), nullptr, Width, Height, frame, true, Mode::Masked);
            expectEqual(framebuffer, expected, "interleaved-mask/y/frame");

            reset(framebuffer, expected);
            graphics.erasePageSprite(9, y, Images.data(), Width, Height, frame);
            referenceBlit(expected, 9, y, Images.data(), nullptr, Width, Height, frame, false, Mode::Erase);
            expectEqual(framebuffer, expected, "erase/y/frame");

            reset(framebuffer, expected);
            graphics.overwritePageSprite(9, y, Images.data(), Width, Height, frame);
            referenceBlit(expected, 9, y, Images.data(), nullptr, Width, Height, frame, false, Mode::Overwrite);
            expectEqual(framebuffer, expected, "overwrite/y/frame");
        }
    }

    const std::array<std::array<int16_t, 2>, 6> clippedPositions = {{{-6, 20}, {-2, -5},
        {124, 20}, {127, 60}, {50, -12}, {50, 58}}};
    for (const auto &position : clippedPositions) {
        reset(framebuffer, expected);
        graphics.drawPageSpriteMasked(
            position[0], position[1], Images.data(), Masks.data(), Width, Height, 2
        );
        referenceBlit(
            expected, position[0], position[1], Images.data(), Masks.data(),
            Width, Height, 2, false, Mode::Masked
        );
        expectEqual(framebuffer, expected, "edge clipping");
    }

    reset(framebuffer, expected);
    graphics.drawPageSpriteMasked(17, 3, Images.data(), Masks.data(), Width, Height, 2, 0);
    referenceBlit(
        expected, 17, 3, Images.data(), Masks.data(), Width, Height,
        2, false, Mode::Masked, 0
    );
    expectEqual(framebuffer, expected, "independent image/mask frames");
}

void testHeaderedAssets()
{
    std::vector<uint8_t> asset = {Width, Height};
    asset.insert(asset.end(), Images.begin(), Images.end());
    std::vector<uint8_t> interleavedAsset = {Width, Height};
    std::vector<uint8_t> maskAsset = {Width, Height};
    maskAsset.insert(maskAsset.end(), Masks.begin(), Masks.end());
    const std::vector<uint8_t> interleaved = interleavedFrames();
    interleavedAsset.insert(interleavedAsset.end(), interleaved.begin(), interleaved.end());

    PoomFramebuffer framebuffer;
    PoomGraphics graphics;
    graphics.attach(&framebuffer);
    Buffer expected{};

    reset(framebuffer, expected);
    graphics.drawPageAsset(117, 7, asset.data(), 1);
    referenceBlit(expected, 117, 7, Images.data(), nullptr, Width, Height, 1, false, Mode::Set);
    expectEqual(framebuffer, expected, "headered asset");

    reset(framebuffer, expected);
    graphics.drawPageAssetMasked(20, 5, asset.data(), maskAsset.data(), 2, 0);
    referenceBlit(
        expected, 20, 5, Images.data(), Masks.data(), Width, Height,
        2, false, Mode::Masked, 0
    );
    expectEqual(framebuffer, expected, "headered separate-mask asset");

    reset(framebuffer, expected);
    graphics.drawPageAssetInterleavedMasked(-2, -1, interleavedAsset.data(), 2);
    referenceBlit(expected, -2, -1, interleaved.data(), nullptr, Width, Height, 2, true, Mode::Masked);
    expectEqual(framebuffer, expected, "headered interleaved asset");

    reset(framebuffer, expected);
    graphics.erasePageAsset(61, 56, asset.data(), 0);
    referenceBlit(expected, 61, 56, Images.data(), nullptr, Width, Height, 0, false, Mode::Erase);
    expectEqual(framebuffer, expected, "headered erase asset");

    reset(framebuffer, expected);
    graphics.overwritePageAsset(5, 1, asset.data(), 2);
    referenceBlit(expected, 5, 1, Images.data(), nullptr, Width, Height, 2, false, Mode::Overwrite);
    expectEqual(framebuffer, expected, "headered overwrite asset");
}

bool rowPixel(const uint8_t *data, uint8_t width, int16_t x, int16_t y)
{
    const size_t rowBytes = (width + 7U) / 8U;
    return (data[static_cast<size_t>(y) * rowBytes + x / 8] & (0x80U >> (x & 7))) != 0;
}

void testExistingRowPackedBehavior()
{
    constexpr uint8_t width = 10;
    constexpr uint8_t height = 5;
    const uint8_t image[] = {0xAA, 0x80, 0x55, 0x40, 0xF0, 0xC0, 0x0F, 0x00, 0x81, 0x00};
    const uint8_t mask[] = {0xFF, 0xC0, 0x7E, 0x40, 0xF3, 0xC0, 0x3F, 0x00, 0xC3, 0xC0};

    PoomFramebuffer framebuffer;
    PoomGraphics graphics;
    graphics.attach(&framebuffer);
    Buffer expected{};
    reset(framebuffer, expected);
    graphics.drawBitmapMasked(-3, 62, image, mask, width, height);

    for (int16_t sourceY = 0; sourceY < height; ++sourceY) {
        for (int16_t sourceX = 0; sourceX < width; ++sourceX) {
            const int16_t destinationX = static_cast<int16_t>(sourceX - 3);
            const int16_t destinationY = static_cast<int16_t>(sourceY + 62);
            if (destinationX < 0 || destinationX >= PoomScreenWidth ||
                destinationY < 0 || destinationY >= PoomScreenHeight ||
                !rowPixel(mask, width, sourceX, sourceY)) {
                continue;
            }
            setPixel(expected, destinationX, destinationY, rowPixel(image, width, sourceX, sourceY));
        }
    }

    expectEqual(framebuffer, expected, "1.0.1 row-packed masked behavior");
}

void requirePixel(
    const PoomFramebuffer &framebuffer,
    int16_t x,
    int16_t y,
    bool expected,
    const char *label
)
{
    if (framebuffer.getPixel(x, y) == expected) {
        return;
    }
    std::cerr << label << " failed at pixel (" << x << ", " << y << ")\n";
    std::exit(1);
}

void testDrawingPrimitives()
{
    PoomFramebuffer framebuffer;
    Buffer expected{};

    const std::array<std::array<int16_t, 3>, 8> verticalLines = {{{0, 0, 64},
        {127, 3, 1}, {64, 6, 19}, {9, -5, 12}, {17, 60, 12}, {-1, 3, 8},
        {128, 3, 8}, {21, 12, 0}}};
    for (const auto &line : verticalLines) {
        for (const bool on : {false, true}) {
            reset(framebuffer, expected);
            framebuffer.drawVerticalLine(line[0], line[1], line[2], on);
            for (int32_t drawY = line[1]; drawY < static_cast<int32_t>(line[1]) + line[2]; ++drawY) {
                if (line[0] >= 0 && line[0] < PoomScreenWidth &&
                    drawY >= 0 && drawY < PoomScreenHeight) {
                    setPixel(expected, line[0], static_cast<int16_t>(drawY), on);
                }
            }
            expectEqual(framebuffer, expected, "vertical line");
        }
    }

    const std::array<std::array<int16_t, 4>, 7> rectangles = {{{0, 0, 128, 64},
        {4, 3, 19, 21}, {-5, -7, 13, 17}, {120, 57, 20, 15},
        {-20, 4, 5, 5}, {8, 70, 5, 5}, {4, 4, 0, 8}}};
    for (const auto &rect : rectangles) {
        for (const bool on : {false, true}) {
            reset(framebuffer, expected);
            framebuffer.fillRect(rect[0], rect[1], rect[2], rect[3], on);
            for (int32_t drawY = rect[1]; drawY < static_cast<int32_t>(rect[1]) + rect[3]; ++drawY) {
                for (int32_t drawX = rect[0]; drawX < static_cast<int32_t>(rect[0]) + rect[2]; ++drawX) {
                    if (drawX >= 0 && drawX < PoomScreenWidth &&
                        drawY >= 0 && drawY < PoomScreenHeight) {
                        setPixel(expected, static_cast<int16_t>(drawX), static_cast<int16_t>(drawY), on);
                    }
                }
            }
            expectEqual(framebuffer, expected, "filled rectangle");
        }
    }

    framebuffer.clear();
    framebuffer.drawLine(2, 3, 8, 6);
    const std::array<std::array<int16_t, 2>, 7> linePixels = {{{2, 3}, {3, 3}, {4, 4},
        {5, 4}, {6, 5}, {7, 5}, {8, 6}}};
    for (const auto &pixel : linePixels) {
        requirePixel(framebuffer, pixel[0], pixel[1], true, "line");
    }
    requirePixel(framebuffer, 3, 4, false, "line gap");

    framebuffer.clear();
    framebuffer.drawCircle(20, 20, 5);
    requirePixel(framebuffer, 20, 15, true, "circle top");
    requirePixel(framebuffer, 25, 20, true, "circle right");
    requirePixel(framebuffer, 20, 20, false, "circle center");
    framebuffer.fillCircle(35, 20, 4);
    requirePixel(framebuffer, 35, 20, true, "filled circle center");
    requirePixel(framebuffer, 39, 20, true, "filled circle edge");
    requirePixel(framebuffer, 40, 20, false, "filled circle outside");

    framebuffer.clear();
    framebuffer.drawRoundRect(5, 5, 20, 12, 3);
    requirePixel(framebuffer, 5, 5, false, "rounded rectangle corner");
    requirePixel(framebuffer, 8, 5, true, "rounded rectangle top");
    requirePixel(framebuffer, 5, 8, true, "rounded rectangle left");
    requirePixel(framebuffer, 15, 10, false, "rounded rectangle center");
    framebuffer.fillRoundRect(30, 5, 20, 12, 3);
    requirePixel(framebuffer, 30, 5, false, "filled rounded rectangle corner");
    requirePixel(framebuffer, 33, 5, true, "filled rounded rectangle top");
    requirePixel(framebuffer, 40, 10, true, "filled rounded rectangle center");

    framebuffer.clear();
    framebuffer.drawTriangle(10, 5, 4, 15, 16, 15);
    requirePixel(framebuffer, 10, 5, true, "triangle vertex");
    requirePixel(framebuffer, 4, 15, true, "triangle base");
    requirePixel(framebuffer, 10, 12, false, "triangle center");
    framebuffer.fillTriangle(30, 5, 24, 15, 36, 15);
    requirePixel(framebuffer, 30, 10, true, "filled triangle center");
    requirePixel(framebuffer, 24, 15, true, "filled triangle base");

    framebuffer.clear();
    framebuffer.drawLine(-5, -5, 5, 5);
    requirePixel(framebuffer, 0, 0, true, "clipped line");
    requirePixel(framebuffer, 5, 5, true, "clipped line endpoint");
}

void testCompressedBitmaps()
{
    const std::vector<uint8_t> compressed =
        compressPageBitmap(Images.data(), Width, Height);
    PoomFramebuffer framebuffer;
    PoomGraphics graphics;
    graphics.attach(&framebuffer);
    Buffer expected{};

    reset(framebuffer, expected);
    graphics.drawCompressed(9, 3, compressed.data());
    referenceBlit(expected, 9, 3, Images.data(), nullptr, Width, Height, 0, false, Mode::Set);
    expectEqual(framebuffer, expected, "compressed set");

    reset(framebuffer, expected);
    graphics.drawCompressed(-3, -5, compressed.data(), false);
    referenceBlit(expected, -3, -5, Images.data(), nullptr, Width, Height, 0, false, Mode::Erase);
    expectEqual(framebuffer, expected, "compressed erase and clipping");

    reset(framebuffer, expected);
    graphics.drawCompressedMirror(120, 54, compressed.data(), true, true);
    for (int16_t sourceY = 0; sourceY < Height; ++sourceY) {
        for (int16_t sourceX = 0; sourceX < Width; ++sourceX) {
            if (!sourcePixel(Images.data(), Width, Height, 0, false, sourceX, sourceY, false)) {
                continue;
            }
            const int16_t destinationX = static_cast<int16_t>(120 + Width - 1 - sourceX);
            const int16_t destinationY = static_cast<int16_t>(54 + sourceY);
            if (destinationX >= 0 && destinationX < PoomScreenWidth &&
                destinationY >= 0 && destinationY < PoomScreenHeight) {
                setPixel(expected, destinationX, destinationY, true);
            }
        }
    }
    expectEqual(framebuffer, expected, "compressed mirror and clipping");

    BitWriter maximumRun;
    maximumRun.write(0xFF, 8); // 256 pixels wide.
    maximumRun.write(0xFF, 8); // 256 pixels high.
    maximumRun.write(1, 1);    // The single span is white.
    maximumRun.write(0, 8);    // Select a 17-bit span length.
    maximumRun.write(1, 1);
    maximumRun.write(65535, 17); // 65536 pixels after adding one.
    framebuffer.clear();
    graphics.drawCompressed(0, 0, maximumRun.bytes().data());
    Buffer fullScreen{};
    fullScreen.fill(0xFF);
    expectEqual(framebuffer, fullScreen, "compressed maximum run length");
}
}

int main()
{
    testPageModesAndClipping();
    testHeaderedAssets();
    testExistingRowPackedBehavior();
    testDrawingPrimitives();
    testCompressedBitmaps();
    std::cout << "All graphics tests passed\n";
    return 0;
}
