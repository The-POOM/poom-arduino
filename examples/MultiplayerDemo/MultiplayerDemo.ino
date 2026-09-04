#include <Poom.h>
#include <PoomMultiplayer.h>

bool radioReady = false;
PoomMultiplayer multiplayer;

void setup()
{
    Poom.begin();
    Poom.setFrameRate(30);
    radioReady = multiplayer.begin("poom_demo", 1);
}

void loop()
{
    if (!Poom.nextFrame()) {
        return;
    }

    multiplayer.update(Poom.buttonState());

    Poom.clear();
    Poom.setCursor(0, 0);
    Poom.println("ESP-NOW DEMO");
    Poom.println(radioReady ? "RADIO OK" : "RADIO ERROR");
    Poom.println(multiplayer.connected() ? "PEER CONNECTED" : "SEARCHING...");

    const uint8_t remote = multiplayer.remoteButtons();
    char line[22];
    snprintf(line, sizeof(line), "REMOTE 0x%02X", remote);
    Poom.println(line);

    if (multiplayer.remotePressed(PoomButtonA)) {
        Poom.println("REMOTE A");
    }
    Poom.show();
}
