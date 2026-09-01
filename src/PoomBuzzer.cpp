#include "PoomBuzzer.h"

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

void PoomBuzzer::tone(uint16_t frequency)
{
    if (!enabled_) {
        return;
    }

    playing_ = false;
    ::tone(POOM_BUZZER_PIN, frequency);
}

void PoomBuzzer::tone(uint16_t frequency, uint32_t duration)
{
    if (!enabled_) {
        return;
    }

    playing_ = false;
    ::tone(POOM_BUZZER_PIN, frequency, duration);
}

void PoomBuzzer::play(const PoomNote *sequence)
{
    sequence_ = sequence;
    sequenceIndex_ = 0;
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
    sequence_ = nullptr;
    sequenceIndex_ = 0;
    ::noTone(POOM_BUZZER_PIN);
}

void PoomBuzzer::startCurrentNote()
{
    const PoomNote *note = sequence_ + sequenceIndex_;
    const uint16_t frequency = pgm_read_word(&note->frequency);
    const uint16_t durationMs = pgm_read_word(&note->durationMs);

    if (durationMs == 0) {
        stop();
        return;
    }

    if (frequency == 0) {
        ::noTone(POOM_BUZZER_PIN);
    } else {
        ::tone(POOM_BUZZER_PIN, frequency);
    }

    noteEndMs_ = millis() + durationMs;
}
