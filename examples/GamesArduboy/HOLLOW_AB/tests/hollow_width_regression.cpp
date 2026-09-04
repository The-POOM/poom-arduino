#include <cassert>
#include <cstdint>

static constexpr uint16_t wrapPhase(uint16_t phase, bool gameOver)
{
    return static_cast<uint16_t>((phase + 2U - gameOver) & 0x03FFU);
}

static constexpr int8_t wrapSignedByte(uint32_t value)
{
    const uint8_t wrapped = static_cast<uint8_t>(value);
    return (wrapped < 0x80U)
            ? static_cast<int8_t>(wrapped)
            : static_cast<int8_t>(static_cast<int16_t>(wrapped) - 256);
}

static constexpr int8_t hollowDistance(uint16_t random15, uint16_t score)
{
    const uint32_t scaled =
            (static_cast<uint32_t>(random15) + 32768U) * score;
    return wrapSignedByte(scaled >> 22);
}

int main()
{
    static_assert(wrapPhase(1022, false) == 0, "phase must wrap at 1024");
    static_assert(wrapPhase(1023, true) == 0, "game-over phase must wrap");
    static_assert(wrapSignedByte(255) == -1, "signed byte wrap changed");
    static_assert(wrapSignedByte(128) == -128, "signed byte wrap changed");
    static_assert(hollowDistance(0, 64) == 0, "minimum spacing changed");
    static_assert(hollowDistance(32767, 64) == 0, "maximum spacing changed");
    static_assert(hollowDistance(32767, 128) == 1, "spacing scale changed");

    // A downward step is a negative jump on AVR.  If this becomes an unsigned
    // byte, interpolation becomes a large positive displacement and the
    // player is clamped to the bottom of the display.
    const int8_t jump = static_cast<int8_t>(32 - 40);
    for (uint8_t move = 0; move <= 8; ++move) {
        const int16_t interpolated = 40 + jump * move / 8 - 8;
        assert(interpolated >= 24 && interpolated <= 32);
    }
}
