#ifndef POOM_BUZZER_H
#define POOM_BUZZER_H

/**
 * @file PoomBuzzer.h
 * @brief Buzzer and non-blocking sound sequence support.
 */

#include <Arduino.h>
#include <stdint.h>

#include "PoomBoardConfig.h"

/**
 * @brief One note in a Poom sound sequence.
 *
 * Use frequency 0 for a rest. A note with durationMs 0 ends the sequence.
 */
struct PoomNote
{
    /** @brief Tone frequency in hertz, or 0 for silence. */
    uint16_t frequency;

    /** @brief Duration of this note in milliseconds. */
    uint16_t durationMs;
};

/**
 * @brief Buzzer and simple non-blocking audio sequencer.
 */
class PoomBuzzer
{
public:
    /** @brief Configure the buzzer pin and stop any active tone. */
    void begin();

    /**
     * @brief Enable or disable buzzer output.
     * @param enabled true to allow sound, false to stop and mute sound.
     */
    void setEnabled(bool enabled);

    /** @return true when buzzer output is enabled. */
    bool enabled() const;

    /**
     * @brief Start a continuous tone.
     * @param frequency Tone frequency in hertz.
     */
    void tone(uint16_t frequency);

    /**
     * @brief Start a tone with a hardware timed duration.
     * @param frequency Tone frequency in hertz.
     * @param duration Duration in milliseconds.
     */
    void tone(uint16_t frequency, uint32_t duration);

    /**
     * @brief Play a non-blocking PoomNote sequence.
     * @param sequence Null-terminated sequence ending with `{0, 0}`.
     */
    void play(const PoomNote *sequence);

    /** @brief Advance the active non-blocking sound sequence. */
    void update();

    /** @return true while a PoomNote sequence is active. */
    bool isPlaying() const;

    /** @brief Stop any active tone or sequence immediately. */
    void stop();

private:
    bool enabled_ = true;
    bool playing_ = false;
    const PoomNote *sequence_ = nullptr;
    uint16_t sequenceIndex_ = 0;
    uint32_t noteEndMs_ = 0;

    void startCurrentNote();
};

#endif
