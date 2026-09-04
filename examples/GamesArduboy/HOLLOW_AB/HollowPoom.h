#pragma once

#include <Arduino.h>
#include <Poom.h>

namespace HollowPoom {

void setTextColor(uint8_t foreground, uint8_t background);
void setCursor(int16_t x, int16_t y);
size_t print(const char *text);
size_t print(const __FlashStringHelper *text);
size_t print(int value);
size_t print(unsigned int value);
size_t print(long value);
size_t print(unsigned long value);
size_t printAt(int16_t x, int16_t y, const char *text);
size_t printAt(int16_t x, int16_t y, const __FlashStringHelper *text);

void playScore(const PoomNote *score, uint8_t priority);
void stopScore();
void updateAudio();

} // namespace HollowPoom
