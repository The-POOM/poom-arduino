#include <Poom.h>

static const PoomNote kReadySound[] = {
    {660, 80},
    {0, 40},
    {880, 80},
    {0, 40},
    {1320, 120},
    {0, 0}
};

void setup()
{
    Poom.begin();

    Poom.clear();
    Poom.setCursor(0, 0);
    Poom.println(F("Buzzer demo"));
    Poom.println(F("non-blocking"));
    Poom.show();

    Poom.audio().play(kReadySound);
}

void loop()
{
    Poom.update();
}
