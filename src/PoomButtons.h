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
 * @brief Bit masks returned by PoomButtons::state().
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

    /** @brief Sample all button pins and advance the button state by one frame. */
    void update();

    /**
     * @brief Get the button state captured by the most recent update().
     * @return Combination of PoomButtonMask bits for pressed buttons.
     */
    uint8_t state() const;

    /**
     * @brief Get the cached button state.
     *
     * Kept as a backwards-compatible alias for state(). This method does not
     * read the hardware; call update() to capture a new sample.
     *
     * @return Combination of PoomButtonMask bits for pressed buttons.
     */
    uint8_t read() const;

    /**
     * @brief Test whether one or more buttons are pressed.
     * @param mask Combination of PoomButtonMask values.
     * @return true when all requested buttons are pressed.
     */
    bool pressed(uint8_t mask) const;

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

private:
    uint8_t currentState_ = 0;
    uint8_t previousState_ = 0;

    uint8_t readHardware() const;
};

#endif
