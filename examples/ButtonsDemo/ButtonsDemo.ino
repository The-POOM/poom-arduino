#include <Poom.h>

void showMessage(const __FlashStringHelper *message)
{
    Poom.clear();
    Poom.setCursor(0, 0);
    Poom.println(message);
    Poom.show();
}

void setup()
{
    Poom.begin();
    Poom.setFrameRate(20);
    showMessage(F("Press a button"));
}

void loop()
{
    if (!Poom.nextFrame()) {
        return;
    }

    if (Poom.pressed(PoomButtonA)) {
        showMessage(F("Button A"));
    } else if (Poom.pressed(PoomButtonB)) {
        showMessage(F("Button B"));
    } else if (Poom.pressed(PoomButtonUp)) {
        showMessage(F("Up"));
    } else if (Poom.pressed(PoomButtonDown)) {
        showMessage(F("Down"));
    } else if (Poom.pressed(PoomButtonLeft)) {
        showMessage(F("Left"));
    } else if (Poom.pressed(PoomButtonRight)) {
        showMessage(F("Right"));
    }
}
