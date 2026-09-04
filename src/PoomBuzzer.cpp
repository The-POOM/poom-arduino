#include "PoomBuzzer.h"
#include "PoomStorage.h"

#if defined(ARDUINO_ARCH_ESP32)
namespace
{
constexpr const char *PoomPreferencesNamespace = "poom";
constexpr const char *PoomAudioEnabledKey = "audio";
}
#endif

void PoomBuzzer::begin()
{
    pinMode(POOM_BUZZER_PIN, OUTPUT);
    stop();
}

void PoomBuzzer::setEnabled(bool enabled)
{
    enabled_ = enabled;
    if (!enabled_) {
        stop();
    }
}

bool PoomBuzzer::enabled() const
{
    return enabled_;
}

bool PoomBuzzer::loadEnabled()
{
#if defined(ARDUINO_ARCH_ESP32)
    PoomStorage storage;
    if (!storage.begin(PoomPreferencesNamespace, true)) {
        return enabled_;
    }

    const bool storedEnabled = storage.readByte(PoomAudioEnabledKey, enabled_ ? 1U : 0U) != 0;
    storage.end();
    setEnabled(storedEnabled);
#endif
    return enabled_;
}

bool PoomBuzzer::saveEnabled() const
{
#if defined(ARDUINO_ARCH_ESP32)
    PoomStorage storage;
    if (!storage.begin(PoomPreferencesNamespace)) {
        return false;
    }

    const bool alreadyStored = storage.contains(PoomAudioEnabledKey) &&
        (storage.readByte(PoomAudioEnabledKey) != 0) == enabled_;
    const bool saved = alreadyStored ||
        storage.writeByte(PoomAudioEnabledKey, enabled_ ? 1U : 0U);
    storage.end();
    return saved;
#else
    return false;
#endif
}

void PoomBuzzer::tone(uint16_t frequency)
{
    if (!enabled_) {
        return;
    }

    playing_ = false;
    repeat_ = false;
    sequence_ = nullptr;
    ::tone(POOM_BUZZER_PIN, frequency);
}

void PoomBuzzer::tone(uint16_t frequency, uint32_t duration)
{
    if (!enabled_) {
        return;
    }

    playing_ = false;
    repeat_ = false;
    sequence_ = nullptr;
    ::tone(POOM_BUZZER_PIN, frequency, duration);
}

void PoomBuzzer::play(const PoomNote *sequence)
{
    play(sequence, false);
}

void PoomBuzzer::play(const PoomNote *sequence, bool repeat)
{
    sequence_ = sequence;
    sequenceIndex_ = 0;
    repeat_ = repeat;
    playing_ = sequence_ != nullptr && enabled_;

    if (!playing_) {
        stop();
        return;
    }

    startCurrentNote();
}

void PoomBuzzer::update()
{
    if (!playing_) {
        return;
    }

    if (!enabled_ || !sequence_) {
        stop();
        return;
    }

    if (static_cast<int32_t>(millis() - noteEndMs_) < 0) {
        return;
    }

    ++sequenceIndex_;
    startCurrentNote();
}

bool PoomBuzzer::isPlaying() const
{
    return playing_;
}

void PoomBuzzer::stop()
{
    playing_ = false;
    repeat_ = false;
    sequence_ = nullptr;
    sequenceIndex_ = 0;
    ::noTone(POOM_BUZZER_PIN);
}

void PoomBuzzer::startCurrentNote()
{
    const PoomNote *note = sequence_ + sequenceIndex_;
    uint16_t frequency = pgm_read_word(&note->frequency);
    uint16_t durationMs = pgm_read_word(&note->durationMs);

    if (durationMs == 0) {
        if (!repeat_ || sequenceIndex_ == 0) {
            stop();
            return;
        }

        sequenceIndex_ = 0;
        note = sequence_;
        frequency = pgm_read_word(&note->frequency);
        durationMs = pgm_read_word(&note->durationMs);
        if (durationMs == 0) {
            stop();
            return;
        }
    }

    if (frequency == 0) {
        ::noTone(POOM_BUZZER_PIN);
    } else {
        ::tone(POOM_BUZZER_PIN, frequency);
    }

    noteEndMs_ = millis() + durationMs;
}
