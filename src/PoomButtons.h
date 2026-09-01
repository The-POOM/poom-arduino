#ifndef POOM_BUTTONS_H
#define POOM_BUTTONS_H

/**
 * @file PoomButtons.h
 * @brief Button masks and button helper class for Poom.
 */

#include <Arduino.h>
#include <stdint.h>

#include "PoomBoardConfig.h"

/**
 * @brief Bit masks returned by PoomButtons::read().
 */
enum PoomButtonMask : uint8_t
{
    /** @brief B/action button. */
    PoomButtonB = 0x01,
    /** @brief A/action button. */
    PoomButtonA = 0x02,
    /** @brief D-pad down. */
    PoomButtonDown = 0x04,
    /** @brief D-pad up. */
    PoomButtonUp = 0x08,
    /** @brief D-pad right. */
    PoomButtonRight = 0x10,
    /** @brief D-pad left. */
    PoomButtonLeft = 0x20
};

/**
 * @brief Button reader for the Poom A/B buttons and D-pad.
 */
class PoomButtons
{
public:
    /** @brief Configure all button pins. */
    void begin();

    /**
     * @brief Read all buttons.
     * @return Combination of PoomButtonMask bits for currently pressed buttons.
     */
    uint8_t read() const;

    /**
     * @brief Test whether one or more buttons are pressed.
     * @param mask Combination of PoomButtonMask values.
     * @return true when all requested buttons are pressed.
     */
    bool pressed(uint8_t mask) const;

    /** @return true when the A button is pressed. */
    bool a() const;

    /** @return true when the B button is pressed. */
    bool b() const;

    /** @return true when the Up button is pressed. */
    bool up() const;

    /** @return true when the Down button is pressed. */
    bool down() const;

    /** @return true when the Left button is pressed. */
    bool left() const;

    /** @return true when the Right button is pressed. */
    bool right() const;
};

#endif
