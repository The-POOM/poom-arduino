#ifndef POOM_GRAPHICS_H
#define POOM_GRAPHICS_H

/**
 * @file PoomGraphics.h
 * @brief Bitmap and sprite drawing helpers for Poom.
 */

#include <stdint.h>

#include "PoomFramebuffer.h"

/**
 * @brief Bitmap and sprite drawing helpers for the Poom framebuffer.
 *
 * The original bitmap APIs use row-packed 1bpp data: each row is stored left
 * to right, bit 7 first, and rows are consecutive. The explicitly named page
 * APIs accept page-packed data matching the framebuffer layout.
 */
class PoomGraphics
{
public:
    /**
     * @brief Attach the framebuffer that drawing operations will modify.
     * @param framebuffer Destination framebuffer. Passing nullptr disables drawing.
     */
    void attach(PoomFramebuffer *framebuffer);

    /**
     * @brief Draw a bitmap using set pixels only.
     * @param x Destination X coordinate.
     * @param y Destination Y coordinate.
     * @param bitmap Row-packed 1bpp bitmap data.
     * @param width Bitmap width in pixels.
     * @param height Bitmap height in pixels.
     */
    void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t width, uint8_t height);

    /**
     * @brief Draw a bitmap through a mask.
     * @param x Destination X coordinate.
     * @param y Destination Y coordinate.
     * @param bitmap Row-packed 1bpp bitmap data.
     * @param mask Row-packed 1bpp mask data; set bits are drawable pixels.
     * @param width Bitmap and mask width in pixels.
     * @param height Bitmap and mask height in pixels.
     */
    void drawBitmapMasked(
        int16_t x,
        int16_t y,
        const uint8_t *bitmap,
        const uint8_t *mask,
        uint8_t width,
        uint8_t height
    );

    /**
     * @brief Draw one frame from consecutive bitmap frames.
     * @param x Destination X coordinate.
     * @param y Destination Y coordinate.
     * @param frames Row-packed 1bpp frame data.
     * @param width Frame width in pixels.
     * @param height Frame height in pixels.
     * @param frame Zero-based frame index.
     */
    void drawSprite(
        int16_t x,
        int16_t y,
        const uint8_t *frames,
        uint8_t width,
        uint8_t height,
        uint8_t frame
    );

    /**
     * @brief Draw one frame using a matching mask frame.
     * @param x Destination X coordinate.
     * @param y Destination Y coordinate.
     * @param frames Row-packed 1bpp frame data.
     * @param masks Row-packed 1bpp mask frame data.
     * @param width Frame width in pixels.
     * @param height Frame height in pixels.
     * @param frame Zero-based frame index.
     */
    void drawSpriteMasked(
        int16_t x,
        int16_t y,
        const uint8_t *frames,
        const uint8_t *masks,
        uint8_t width,
        uint8_t height,
        uint8_t frame
    );

    /**
     * @brief Draw a page-packed bitmap using set pixels only.
     * @param x Destination X coordinate.
     * @param y Destination Y coordinate.
     * @param bitmap Page-packed 1bpp bitmap data, with one vertical byte per column.
     * @param width Bitmap width in pixels.
     * @param height Bitmap height in pixels.
     */
    void drawPageBitmap(
        int16_t x,
        int16_t y,
        const uint8_t *bitmap,
        uint8_t width,
        uint8_t height
    );

    /**
     * @brief Draw a page-packed bitmap through a separate page-packed mask.
     * @param x Destination X coordinate.
     * @param y Destination Y coordinate.
     * @param bitmap Page-packed 1bpp bitmap data.
     * @param mask Page-packed mask data; set bits are drawable pixels.
     * @param width Bitmap and mask width in pixels.
     * @param height Bitmap and mask height in pixels.
     */
    void drawPageBitmapMasked(
        int16_t x,
        int16_t y,
        const uint8_t *bitmap,
        const uint8_t *mask,
        uint8_t width,
        uint8_t height
    );

    /**
     * @brief Draw one frame from consecutive page-packed frames.
     * @param x Destination X coordinate.
     * @param y Destination Y coordinate.
     * @param frames Consecutive page-packed 1bpp frame data.
     * @param width Frame width in pixels.
     * @param height Frame height in pixels.
     * @param frame Zero-based frame index.
     */
    void drawPageSprite(
        int16_t x,
        int16_t y,
        const uint8_t *frames,
        uint8_t width,
        uint8_t height,
        uint8_t frame
    );

    /**
     * @brief Draw one page-packed frame through a separate mask frame.
     * @param x Destination X coordinate.
     * @param y Destination Y coordinate.
     * @param frames Consecutive page-packed image frames.
     * @param masks Consecutive page-packed mask frames.
     * @param width Frame width in pixels.
     * @param height Frame height in pixels.
     * @param frame Zero-based frame index.
     */
    void drawPageSpriteMasked(
        int16_t x,
        int16_t y,
        const uint8_t *frames,
        const uint8_t *masks,
        uint8_t width,
        uint8_t height,
        uint8_t frame
    );

    /** @brief Draw an image frame through an independently selected mask frame. */
    void drawPageSpriteMasked(
        int16_t x,
        int16_t y,
        const uint8_t *frames,
        const uint8_t *masks,
        uint8_t width,
        uint8_t height,
        uint8_t imageFrame,
        uint8_t maskFrame
    );

    /**
     * @brief Draw one page-packed frame stored as alternating image/mask bytes.
     */
    void drawPageSpriteInterleavedMasked(
        int16_t x,
        int16_t y,
        const uint8_t *frames,
        uint8_t width,
        uint8_t height,
        uint8_t frame
    );

    /** @brief Clear pixels set in one page-packed sprite frame. */
    void erasePageSprite(
        int16_t x,
        int16_t y,
        const uint8_t *frames,
        uint8_t width,
        uint8_t height,
        uint8_t frame
    );

    /** @brief Replace every pixel in one page-packed sprite rectangle. */
    void overwritePageSprite(
        int16_t x,
        int16_t y,
        const uint8_t *frames,
        uint8_t width,
        uint8_t height,
        uint8_t frame
    );

    /**
     * @brief Draw a frame from an asset beginning with width and height bytes.
     */
    void drawPageAsset(int16_t x, int16_t y, const uint8_t *asset, uint8_t frame = 0);

    /** @brief Draw a headered image asset through a separate headered mask asset. */
    void drawPageAssetMasked(
        int16_t x,
        int16_t y,
        const uint8_t *asset,
        const uint8_t *mask,
        uint8_t imageFrame = 0,
        uint8_t maskFrame = 0
    );

    /**
     * @brief Draw a frame from a headered asset containing image/mask byte pairs.
     */
    void drawPageAssetInterleavedMasked(
        int16_t x,
        int16_t y,
        const uint8_t *asset,
        uint8_t frame = 0
    );

    /** @brief Clear set pixels from a frame in a headered page-packed asset. */
    void erasePageAsset(int16_t x, int16_t y, const uint8_t *asset, uint8_t frame = 0);

    /** @brief Replace the rectangle for a frame in a headered page-packed asset. */
    void overwritePageAsset(int16_t x, int16_t y, const uint8_t *asset, uint8_t frame = 0);

    /**
     * @brief Draw an Arduboy-compatible compressed bitmap.
     *
     * Set bits are applied using @p on. Clear source bits leave the existing
     * framebuffer unchanged, matching Arduboy2 drawCompressed().
     */
    void drawCompressed(int16_t x, int16_t y, const uint8_t *bitmap, bool on = true);

    /** @brief Draw an Arduboy-compatible compressed bitmap with optional horizontal mirroring. */
    void drawCompressedMirror(
        int16_t x,
        int16_t y,
        const uint8_t *bitmap,
        bool on = true,
        bool mirror = false
    );

    /**
     * @brief Calculate the byte size of one row-packed frame.
     * @param width Frame width in pixels.
     * @param height Frame height in pixels.
     * @return Number of bytes used by one frame.
     */
    size_t frameSize(uint8_t width, uint8_t height) const;

private:
    enum class PageBlitMode : uint8_t
    {
        Set,
        Masked,
        Erase,
        Overwrite
    };

    PoomFramebuffer *framebuffer_ = nullptr;

    bool isReady() const;
    bool sampleBit(const uint8_t *bitmap, uint8_t width, int16_t x, int16_t y) const;
    size_t pageFrameSize(uint8_t width, uint8_t height) const;
    void drawBitmapInternal(
        int16_t x,
        int16_t y,
        const uint8_t *bitmap,
        const uint8_t *mask,
        uint8_t width,
        uint8_t height,
        bool overwrite
    );
    void drawPageBitmapInternal(
        int16_t x,
        int16_t y,
        const uint8_t *bitmap,
        const uint8_t *mask,
        uint8_t width,
        uint8_t height,
        uint8_t imageFrame,
        uint8_t maskFrame,
        bool interleaved,
        PageBlitMode mode
    );
    void drawSpriteMaskedInternal(
        int16_t x,
        int16_t y,
        const uint8_t *frames,
        const uint8_t *masks,
        uint8_t width,
        uint8_t height,
        uint8_t frame
    );
};

#endif
