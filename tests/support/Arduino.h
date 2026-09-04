#ifndef ARDUINO_H
#define ARDUINO_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>

class __FlashStringHelper;

constexpr uint8_t OUTPUT = 1;

void pinMode(uint8_t pin, uint8_t mode);
unsigned long millis();
unsigned long micros();
void delay(unsigned long duration);
void tone(uint8_t pin, unsigned int frequency);
void tone(uint8_t pin, unsigned int frequency, unsigned long duration);
void noTone(uint8_t pin);

#ifndef PROGMEM
#define PROGMEM
#endif

#endif
