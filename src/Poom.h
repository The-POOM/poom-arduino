#ifndef POOM_H
#define POOM_H

/**
 * @file Poom.h
 * @brief Main include file and high-level Poom API.
 */

#include "PoomBoardConfig.h"
#include "PoomButtons.h"
#include "PoomBuzzer.h"
#include "PoomDisplay.h"
#include "PoomFramebuffer.h"
#include "PoomGraphics.h"
#include "PoomLeds.h"
#include "PoomStorage.h"

/**
 * @brief Main facade for the Poom Arduino library.
 *
 * Use the global ::Poom instance from sketches. The facade owns the display,
 * framebuffer, graphics helpers, buttons, buzzer/audio, and LEDs, and keeps the
 * common game loop API compact.
 */
class PoomClass
{
public:
    /**
     * @brief Initialize all supported Poom hardware modules.
     * @return true when the display initializes successfully, false otherwise.
     */
    bool begin();

    /** @brief Clear the Poom framebuffer and the display driver's staging area. */
    void clear();

    /**
     * @brief Fill the whole framebuffer.
     * @param on true for white pixels, false for black pixels.
     */
    void fill(bool on);

    /** @brief Invert every pixel currently stored in the framebuffer. */
    void invert();

    /** @brief Flush the Poom framebuffer to the OLED display. */
    void show();

    /**
     * @brief Get writable access to the raw 1bpp framebuffer.
     * @return Pointer to PoomFramebufferSize bytes in page layout.
     */
    uint8_t *buffer();

    /**
     * @brief Get read-only access to the raw 1bpp framebuffer.
     * @return Pointer to PoomFramebufferSize bytes in page layout.
     */
    const uint8_t *buffer() const;

    /**
     * @brief Draw one pixel into the framebuffer.
     * @param x Horizontal position, starting at 0.
     * @param y Vertical position, starting at 0.
     * @param on true to set the pixel, false to clear it.
     */
    void drawPixel(int16_t x, int16_t y, bool on = true);

    /** @brief Draw a clipped horizontal line into the framebuffer. */
    void drawHorizontalLine(int16_t x, int16_t y, int16_t width, bool on = true);

    /** @brief Draw a clipped vertical line into the framebuffer. */
    void drawVerticalLine(int16_t x, int16_t y, int16_t height, bool on = true);

    /** @brief Draw a clipped line between two endpoints. */
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, bool on = true);

    /** @brief Draw a clipped circle outline. */
    void drawCircle(int16_t x, int16_t y, int16_t radius, bool on = true);

    /** @brief Draw a clipped filled circle. */
    void fillCircle(int16_t x, int16_t y, int16_t radius, bool on = true);

    /** @brief Draw a clipped filled rectangle into the framebuffer. */
    void fillRect(int16_t x, int16_t y, int16_t width, int16_t height, bool on = true);

    /** @brief Draw a clipped rectangle outline into the framebuffer. */
    void drawRect(int16_t x, int16_t y, int16_t width, int16_t height, bool on = true);

    /** @brief Draw a clipped rounded rectangle outline. */
    void drawRoundRect(
        int16_t x,
        int16_t y,
        int16_t width,
        int16_t height,
        int16_t radius,
        bool on = true
    );

    /** @brief Draw a clipped filled rounded rectangle. */
    void fillRoundRect(
        int16_t x,
        int16_t y,
        int16_t width,
        int16_t height,
        int16_t radius,
        bool on = true
    );

    /** @brief Draw a clipped triangle outline. */
    void drawTriangle(
        int16_t x0,
        int16_t y0,
        int16_t x1,
        int16_t y1,
        int16_t x2,
        int16_t y2,
        bool on = true
    );

    /** @brief Draw a clipped filled triangle. */
    void fillTriangle(
        int16_t x0,
        int16_t y0,
        int16_t x1,
        int16_t y1,
        int16_t x2,
        int16_t y2,
        bool on = true
    );

    /**
     * @brief Read one framebuffer pixel.
     * @param x Horizontal position, starting at 0.
     * @param y Vertical position, starting at 0.
     * @return true when the pixel is set, false when clear or outside bounds.
     */
    bool getPixel(int16_t x, int16_t y) const;

    /**
     * @brief Draw a row-packed 1bpp bitmap into the framebuffer.
     * @param x Destination X coordinate.
     * @param y Destination Y coordinate.
     * @param bitmap Bitmap data in Poom graphics format.
     * @param width Bitmap width in pixels.
     * @param height Bitmap height in pixels.
     */
    void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t width, uint8_t height);

    /**
     * @brief Draw a row-packed 1bpp bitmap using a separate mask.
     * @param x Destination X coordinate.
     * @param y Destination Y coordinate.
     * @param bitmap Bitmap data in Poom graphics format.
     * @param mask Mask data in the same format; set bits are drawable pixels.
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
     * @brief Draw one frame from a packed sprite sheet.
     * @param x Destination X coordinate.
     * @param y Destination Y coordinate.
     * @param frames Consecutive bitmap frames in Poom graphics format.
     * @param width Frame width in pixels.
     * @param height Frame height in pixels.
     * @param frame Zero-based frame index.
     */
    void drawSprite(int16_t x, int16_t y, const uint8_t *frames, uint8_t width, uint8_t height, uint8_t frame);

    /**
     * @brief Draw one masked frame from packed sprite and mask sheets.
     * @param x Destination X coordinate.
     * @param y Destination Y coordinate.
     * @param frames Consecutive bitmap frames in Poom graphics format.
     * @param masks Consecutive mask frames in Poom graphics format.
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
     * @brief Draw a page-packed 1bpp bitmap into the framebuffer.
     * @param x Destination X coordinate.
     * @param y Destination Y coordinate.
     * @param bitmap Page-packed bitmap data, with one vertical byte per column.
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
     * @brief Draw a page-packed 1bpp bitmap through a separate mask.
     * @param x Destination X coordinate.
     * @param y Destination Y coordinate.
     * @param bitmap Page-packed bitmap data.
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
     * @param frames Consecutive page-packed frame data.
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

    /** @brief Draw one page-packed frame through a separate mask frame. */
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

    /** @brief Draw one frame stored as alternating page-packed image/mask bytes. */
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

    /** @brief Draw a frame from a page-packed asset with a width/height header. */
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

    /** @brief Draw a frame from a headered asset containing image/mask byte pairs. */
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

    /** @brief Draw an Arduboy-compatible compressed bitmap. */
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
     * @brief Set the text cursor used by print() and println().
     * @param x Cursor X coordinate in pixels.
     * @param y Cursor Y coordinate in pixels.
     */
    void setCursor(int16_t x, int16_t y);

    /**
     * @brief Set the framebuffer text scale.
     * @param size Pixel scale. Values less than 1 are treated as 1.
     */
    void setTextSize(uint8_t size);

    /** @brief Print a RAM string into the framebuffer at the current cursor. */
    void print(const char *text);

    /** @brief Print a flash string into the framebuffer at the current cursor. */
    void print(const __FlashStringHelper *text);

    /** @brief Print a RAM string and advance to the next text line. */
    void println(const char *text);

    /** @brief Print a flash string and advance to the next text line. */
    void println(const __FlashStringHelper *text);

    /**
     * @brief Set the target frame rate used by nextFrame().
     * @param framesPerSecond Target frames per second. Zero is treated as 1.
     */
    void setFrameRate(uint8_t framesPerSecond);

    /**
     * @brief Check whether it is time to render the next frame.
     * @return true when the sketch should update and draw a frame.
     */
    bool nextFrame();

    /** @return Number of frames accepted by nextFrame() since begin(). */
    uint32_t frameCount() const;

    /** @return Configured fixed update rate in frames per second. */
    uint8_t targetFrameRate() const;

    /** @return Measured accepted update rate over the latest one-second window. */
    uint16_t measuredFrameRate() const;

    /** @return Number of update deadlines discarded after excessive stalls. */
    uint32_t droppedFrameCount() const;

    /** @return Lateness of the most recently accepted update in microseconds. */
    uint32_t lastFrameLatenessMicros() const;

    /**
     * @brief Test whether the current frame is an interval boundary.
     * @return false for interval zero; otherwise frameCount() modulo interval.
     */
    bool everyFrames(uint16_t interval) const;

    /** @brief Advance non-blocking services such as audio playback. */
    void update();

    /** @return true when this application is running from POOM's OTA1 game slot. */
    bool runningFromLauncher() const;

    /**
     * @brief Enable or disable the default A+B+Down hold-to-exit chord.
     *
     * The chord is only active when the application runs from the OTA1 game
     * slot. It is enabled automatically in that slot by begin().
     */
    void enableExitChord(bool enabled = true);

    /** @return true when automatic hold-to-exit handling is active. */
    bool exitChordEnabled() const;

    /**
     * @brief Restart an OTA1 game so the POOM boot policy returns to the menu.
     * @return false when not running from the launcher; does not return on success.
     */
    bool returnToLauncher();

    /**
     * @brief Test whether one or more buttons are currently pressed.
     * @param mask Combination of PoomButtonMask values.
     * @return true when all requested buttons are pressed.
     */
    bool pressed(uint8_t mask) const;

    /** @return Button state captured at the start of the current frame. */
    uint8_t buttonState() const;

    /**
     * @brief Test whether any requested button is pressed.
     * @param mask Combination of PoomButtonMask values.
     * @return true when at least one requested button is pressed.
     */
    bool anyPressed(uint8_t mask) const;

    /**
     * @brief Test whether any requested button was pressed this frame.
     * @param mask Combination of PoomButtonMask values.
     * @return true when at least one requested button changed to pressed.
     */
    bool justPressed(uint8_t mask) const;

    /**
     * @brief Test whether any requested button was released this frame.
     * @param mask Combination of PoomButtonMask values.
     * @return true when at least one requested button changed to released.
     */
    bool justReleased(uint8_t mask) const;

    /** @brief Access the OLED display driver wrapper. */
    PoomDisplay &screen();

    /** @brief Access the 1bpp framebuffer object. */
    PoomFramebuffer &framebuffer();

    /** @brief Access bitmap and sprite drawing helpers. */
    PoomGraphics &graphics();

    /** @brief Access the non-blocking audio/buzzer helper. */
    PoomBuzzer &audio();

    /** @brief Access button helpers. */
    PoomButtons &buttons();

    /** @brief Access the buzzer helper. */
    PoomBuzzer &buzzer();

    /** @brief Access onboard LED helpers. */
    PoomLeds &leds();

    /** @brief Access namespaced ESP32 Preferences storage. */
    PoomStorage &storage();

private:
    bool frameTimerStarted_ = false;
    bool frameMetricsStarted_ = false;
    uint32_t nextFrameDeadlineUs_ = 0;
    uint32_t frameCount_ = 0;
    uint32_t framePeriodUs_ = 1000000UL / 60U;
    uint32_t framePeriodRemainder_ = 1000000UL % 60U;
    uint32_t frameRemainderAccumulator_ = 0;
    uint32_t droppedFrameCount_ = 0;
    uint32_t lastFrameLatenessUs_ = 0;
    uint32_t frameMetricsStartUs_ = 0;
    uint16_t frameMetricsCount_ = 0;
    uint16_t measuredFrameRate_ = 0;
    uint8_t targetFrameRate_ = 60;
    bool runningFromLauncher_ = false;
    bool exitChordEnabled_ = false;
    bool exitChordHeld_ = false;
    uint32_t exitChordStartedMs_ = 0;
    PoomDisplay display_;
    PoomFramebuffer framebuffer_;
    PoomGraphics graphics_;
    PoomButtons buttons_;
    PoomBuzzer buzzer_;
    PoomLeds leds_;
    PoomStorage storage_;

    void advanceFrameDeadline();
    void recordAcceptedFrame(uint32_t nowUs);
    void updateExitChord(uint32_t now);
};

/** @brief Global Poom library instance used by sketches. */
extern PoomClass Poom;

#endif
