#include "ArduboyTonesFX.h"

namespace
{
uint24_t sequenceStart = 0;
uint24_t sequenceCursor = 0;
uint32_t noteEndMs = 0;
bool sequencePlaying = false;
bool noteHasDuration = false;

void startNextTone(uint32_t scheduledStartMs)
{
    while (sequencePlaying) {
        FX::seekData(sequenceCursor);
        uint16_t frequency = FX::readPendingUInt16();
        sequenceCursor = static_cast<uint24_t>(sequenceCursor + 2U);

        if (frequency == TONES_END) {
            sequencePlaying = false;
            Poom.audio().stop();
            return;
        }
        if (frequency == TONES_REPEAT) {
            sequenceCursor = sequenceStart;
            continue;
        }

        FX::seekData(sequenceCursor);
        const uint16_t duration = FX::readPendingUInt16();
        sequenceCursor = static_cast<uint24_t>(sequenceCursor + 2U);
        frequency &= static_cast<uint16_t>(~TONE_HIGH_VOLUME);

        if (frequency == 0) {
            Poom.audio().stop();
        } else {
            Poom.audio().tone(frequency);
        }

        noteHasDuration = duration != 0;
        noteEndMs = scheduledStartMs + duration;
        return;
    }
}
}

ArduboyTonesFX::ArduboyTonesFX(boolean (*)(), uint16_t *, uint8_t)
{
}

void ArduboyTonesFX::tonesFromFX(uint24_t tones)
{
    sequenceStart = tones;
    sequenceCursor = tones;
    sequencePlaying = true;
    startNextTone(millis());
}

void ArduboyTonesFX::fillBufferFromFX()
{
    if (!sequencePlaying || !noteHasDuration) {
        return;
    }
    const uint32_t nowMs = millis();
    while (sequencePlaying && noteHasDuration &&
           static_cast<int32_t>(nowMs - noteEndMs) >= 0) {
        const uint32_t scheduledStartMs = noteEndMs;
        startNextTone(scheduledStartMs);
    }
}

void ArduboyTonesFX::noTone()
{
    sequencePlaying = false;
    noteHasDuration = false;
    Poom.audio().stop();
}

void ArduboyTonesFX::volumeMode(uint8_t)
{
}

bool ArduboyTonesFX::playing()
{
    return sequencePlaying;
}

void ArduboyTonesFX::nextTone()
{
    startNextTone(millis());
}

uint16_t ArduboyTonesFX::getNext()
{
    FX::seekData(sequenceCursor);
    const uint16_t value = FX::readPendingUInt16();
    sequenceCursor = static_cast<uint24_t>(sequenceCursor + 2U);
    return value;
}
