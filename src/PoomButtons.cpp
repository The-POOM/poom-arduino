#include "PoomButtons.h"

void PoomButtons::begin()
{
    pinMode(POOM_BUTTON_LEFT_PIN, INPUT_PULLUP);
    pinMode(POOM_BUTTON_RIGHT_PIN, INPUT_PULLUP);
    pinMode(POOM_BUTTON_UP_PIN, INPUT_PULLUP);
    pinMode(POOM_BUTTON_DOWN_PIN, INPUT_PULLUP);
    pinMode(POOM_BUTTON_A_PIN, INPUT_PULLUP);
    pinMode(POOM_BUTTON_B_PIN, INPUT_PULLUP);

    currentState_ = readHardware();
    previousState_ = currentState_;
}

void PoomButtons::update()
{
    previousState_ = currentState_;
    currentState_ = readHardware();
}

uint8_t PoomButtons::state() const
{
    return currentState_;
}

uint8_t PoomButtons::read() const
{
    return state();
}

bool PoomButtons::pressed(uint8_t mask) const
{
    return (currentState_ & mask) == mask;
}

bool PoomButtons::anyPressed(uint8_t mask) const
{
    return (currentState_ & mask) != 0;
}

bool PoomButtons::justPressed(uint8_t mask) const
{
    return (currentState_ & static_cast<uint8_t>(~previousState_) & mask) != 0;
}

bool PoomButtons::justReleased(uint8_t mask) const
{
    return (previousState_ & static_cast<uint8_t>(~currentState_) & mask) != 0;
}

uint8_t PoomButtons::readHardware() const
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
