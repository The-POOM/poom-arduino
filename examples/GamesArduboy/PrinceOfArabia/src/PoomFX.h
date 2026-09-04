#pragma once

#include "PoomGameCompat.h"

constexpr uint8_t dbfWhiteBlack = 0;
constexpr uint8_t dbfInvert = 1;
constexpr uint8_t dbfBlack = 2;
constexpr uint8_t dbfReverseBlack = 3;
constexpr uint8_t dbfMasked = 4;
constexpr uint8_t dbfFlip = 5;
constexpr uint8_t dbfEndFrame = 6;
constexpr uint8_t dbfLastFrame = 7;

constexpr uint8_t dbmBlack =
    (1U << dbfReverseBlack) | (1U << dbfBlack) | (1U << dbfWhiteBlack);
constexpr uint8_t dbmWhite = (1U << dbfWhiteBlack);
constexpr uint8_t dbmInvert = (1U << dbfInvert);
constexpr uint8_t dbmFlip = (1U << dbfFlip);
constexpr uint8_t dbmNormal = 0;
constexpr uint8_t dbmOverwrite = 0;
constexpr uint8_t dbmReverse = (1U << dbfReverseBlack);
constexpr uint8_t dbmMasked = (1U << dbfMasked);
constexpr uint8_t dbmEndFrame = (1U << dbfEndFrame);
constexpr uint8_t dbmLastFrame = (1U << dbfLastFrame);

struct FrameControl
{
    uint24_t start;
    uint24_t current;
    uint8_t repeat;
    uint8_t count;
};

class FX
{
public:
    // FX data files always encode addresses in three bytes. On ESP32 the
    // compatibility uint24_t is backed by uint32_t, so sizeof(uint24_t) must
    // never be used to calculate an on-disk table stride.
    static constexpr uint8_t DataAddressSize = 3;

    static void begin();
    static void begin(uint16_t dataPage);
    static void begin(uint16_t dataPage, uint16_t savePage);

    static void enableOLED() {}
    static void display();
    static void display(bool clearBuffer);
    static void configurePresentationTiming(uint8_t updateRate, uint8_t fallbackRate);

    static void seekData(uint24_t address);
    static void seekDataArray(
        uint24_t address,
        uint8_t index,
        uint8_t offset,
        uint8_t elementSize
    );

    static uint8_t readPendingUInt8();
    static uint16_t readPendingUInt16();
    static uint16_t readPendingLastUInt16();
    static uint32_t readPendingLastUInt32();
    static uint8_t readEnd();
    static void readBytes(uint8_t *buffer, size_t length);
    static uint24_t readIndexedUInt24(uint24_t address, uint8_t index);

    static void saveGameState(const uint8_t *gameState, size_t size);
    static uint8_t loadGameState(uint8_t *gameState, size_t size);

    template<typename T>
    static void saveGameState(const T &gameState)
    {
        saveGameState(reinterpret_cast<const uint8_t *>(&gameState), sizeof(T));
    }

    template<typename T>
    static uint8_t loadGameState(T &gameState)
    {
        return loadGameState(reinterpret_cast<uint8_t *>(&gameState), sizeof(T));
    }

    static void drawBitmap(
        int16_t x,
        int16_t y,
        uint24_t address,
        uint8_t frame,
        uint8_t mode
    );

    static void setFrame(uint24_t frame, uint8_t repeat);
    static uint8_t drawFrame();
    static uint24_t drawFrame(uint24_t address);

private:
    static uint32_t readCursor_;
    static FrameControl frameControl_;
    static uint16_t presentationAccumulator_;
    static uint16_t slowPresentationTicks_;
    static uint8_t updateRate_;
    static uint8_t fallbackPresentationRate_;

    static void presentFramebuffer();
    static uint8_t readDataByte(uint32_t address);
    static uint16_t readUInt16();
    static uint24_t readUInt24();
};
