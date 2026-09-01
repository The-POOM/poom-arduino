#include <Poom.h>

void setup()
{
    Poom.begin();

    Poom.clear();
    Poom.setCursor(0, 0);
    Poom.println(F("Poom readyyy!"));
    Poom.show();
}

void loop()
{
}
