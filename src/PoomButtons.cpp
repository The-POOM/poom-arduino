#include "PoomButtons.h"

void PoomButtons::begin()
{
    pinMode(POOM_BUTTON_LEFT_PIN, INPUT_PULLUP);
    pinMode(POOM_BUTTON_RIGHT_PIN, INPUT_PULLUP);
    pinMode(POOM_BUTTON_UP_PIN, INPUT_PULLUP);
    pinMode(POOM_BUTTON_DOWN_PIN, INPUT_PULLUP);
    pinMode(POOM_BUTTON_A_PIN, INPUT_PULLUP);
    pinMode(POOM_BUTTON_B_PIN, INPUT_PULLUP);
}

uint8_t PoomButtons::read() const
{
    uint8_t state = 0;

    if (!digitalRead(POOM_BUTTON_B_PIN)) {
        state |= PoomButtonB;
    }
    if (!digitalRead(POOM_BUTTON_A_PIN)) {
        state |= PoomButtonA;
    }
    if (!digitalRead(POOM_BUTTON_DOWN_PIN)) {
        state |= PoomButtonDown;
    }
    if (!digitalRead(POOM_BUTTON_UP_PIN)) {
        state |= PoomButtonUp;
    }
    if (!digitalRead(POOM_BUTTON_RIGHT_PIN)) {
        state |= PoomButtonRight;
    }
    if (!digitalRead(POOM_BUTTON_LEFT_PIN)) {
        state |= PoomButtonLeft;
    }

    return state;
}

bool PoomButtons::pressed(uint8_t mask) const
{
    return (read() & mask) == mask;
}

bool PoomButtons::a() const
{
    return pressed(PoomButtonA);
}

bool PoomButtons::b() const
{
    return pressed(PoomButtonB);
}

bool PoomButtons::up() const
{
    return pressed(PoomButtonUp);
}

bool PoomButtons::down() const
{
    return pressed(PoomButtonDown);
}

bool PoomButtons::left() const
{
    return pressed(PoomButtonLeft);
}

bool PoomButtons::right() const
{
    return pressed(PoomButtonRight);
}
