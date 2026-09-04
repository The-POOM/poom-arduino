#ifndef COMMON_H
#define COMMON_H

#include <Poom.h>
#include "HollowPoom.h"

/*  Defines  */

#define APP_TITLE       "HOLLOW SEEKER"
#define APP_CODE        "OBN-Y01"
#define APP_VERSION     "0.33"
#define APP_RELEASED    "DECEMBER 2016"

#define mod(value, div) (((value) + div) % div)

/*  Typedefs  */

// The original target is AVR, where uchar is exactly 8 bits.  The original
// `uint` alias cannot be retained: ESP32's system headers already reserve that
// name for a 32-bit unsigned int, so 16-bit game values use uint16_t directly.
typedef uint8_t         uchar;

static_assert(sizeof(uchar) == 1, "Hollow Seeker requires 8-bit uchar");

// avr-libc rand() only returns 0..32767.  ESP32 rand() is much wider, which
// breaks the cave hollow-distance formula even when its result is narrowed
// back to int8_t.  Arduino random() lets us reproduce the required 15-bit
// input range while retaining a well-seeded generator on POOM hardware.
static inline uint16_t avrRand15(void)
{
    return static_cast<uint16_t>(random(32768L));
}

#define rnd(val)        (avrRand15() % (val))

/*  Global Functions  */

void initLogo(void);
bool updateLogo(void);
void drawLogo(void);

void initTitle(void);
bool updateTitle(void);
void drawTitle(void);
uint8_t setLastScore(int score, uint32_t time);

void initGame(void);
bool updateGame(void);
void drawGame(void);

#endif // COMMON_H
