#include <Poom.h>

static const uint8_t kPoomFaceFrames[] PROGMEM = {
    0x3C,
    0x42,
    0xA5,
    0x81,
    0xA5,
    0x99,
    0x42,
    0x3C,

    0x3C,
    0x42,
    0xA5,
    0x81,
    0xBD,
    0x81,
    0x42,
    0x3C
};

void setup()
{
    Poom.begin();
    Poom.setFrameRate(4);
}

void loop()
{
    if (!Poom.nextFrame()) {
        return;
    }

    const uint8_t frame = (millis() / 500) % 2;

    Poom.clear();
    Poom.setCursor(0, 0);
    Poom.println(F("Sprite demo"));
    Poom.drawSprite(60, 28, kPoomFaceFrames, 8, 8, frame);
    Poom.show();
}
