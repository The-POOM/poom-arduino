#include <cstdlib>
#include <iostream>

#include "PoomBuzzer.h"

namespace
{
unsigned long nowMs = 0;
unsigned int activeFrequency = 0;
unsigned long activeDuration = 0;
unsigned toneCalls = 0;
unsigned noToneCalls = 0;

void expect(bool condition, const char *message)
{
    if (!condition) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}
}

void pinMode(uint8_t, uint8_t)
{
}

unsigned long millis()
{
    return nowMs;
}

void tone(uint8_t, unsigned int frequency)
{
    activeFrequency = frequency;
    activeDuration = 0;
    ++toneCalls;
}

void tone(uint8_t, unsigned int frequency, unsigned long duration)
{
    activeFrequency = frequency;
    activeDuration = duration;
    ++toneCalls;
}

void noTone(uint8_t)
{
    activeFrequency = 0;
    ++noToneCalls;
}

int main()
{
    const PoomNote sequence[] = {{440, 10}, {0, 5}, {0, 0}};
    const PoomNote empty[] = {{0, 0}};
    PoomBuzzer buzzer;
    buzzer.begin();

    buzzer.play(sequence, true);
    expect(buzzer.isPlaying(), "looping sequence did not start");
    expect(activeFrequency == 440, "first looping note did not sound");

    nowMs = 10;
    buzzer.update();
    expect(buzzer.isPlaying() && activeFrequency == 0, "rest did not advance");

    nowMs = 15;
    buzzer.update();
    expect(buzzer.isPlaying() && activeFrequency == 440, "sequence did not loop");

    buzzer.play(sequence);
    nowMs = 25;
    buzzer.update();
    nowMs = 30;
    buzzer.update();
    expect(!buzzer.isPlaying(), "non-looping sequence did not stop");

    buzzer.play(empty, true);
    expect(!buzzer.isPlaying(), "empty looping sequence should stop");

    buzzer.setEnabled(false);
    const unsigned callsBeforeMutedTone = toneCalls;
    buzzer.tone(880, 20);
    expect(toneCalls == callsBeforeMutedTone, "disabled buzzer emitted a tone");
    expect(noToneCalls > 0 && activeDuration == 0, "buzzer stop state was not recorded");

    std::cout << "All buzzer tests passed\n";
    return 0;
}
