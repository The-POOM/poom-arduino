#include <Poom.h>

static uint8_t colorStep = 0;
static uint32_t lastChangeMs = 0;

void setup()
{
    Poom.begin();
    Poom.clear();
    Poom.setCursor(0, 0);
    Poom.println(F("LED demo"));
    Poom.show();
}

void loop()
{
    const uint32_t now = millis();
    if (now - lastChangeMs < 600) {
        return;
    }

    lastChangeMs = now;

    if (colorStep == 0) {
        Poom.leds().setColor(24, 0, 0);
    } else if (colorStep == 1) {
        Poom.leds().setColor(0, 24, 0);
    } else if (colorStep == 2) {
        Poom.leds().setColor(0, 0, 24);
    } else {
        Poom.leds().off();
    }

    colorStep = (colorStep + 1) % 4;
}
