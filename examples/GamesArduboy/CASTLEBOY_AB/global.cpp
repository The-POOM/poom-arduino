#include "global.h"

#include "assets.h"

uint8_t mainState;
uint8_t flashCounter = 0;

const PoomNote SoundBossStart[] PROGMEM = {
  {NoteG4, 300}, {NoteG3, 300}, {NoteG2, 900}, {0, 0}
};
const PoomNote SoundPlayerDied[] PROGMEM = {
  {NoteG3, 100}, {NoteG2, 150}, {NoteG1, 350}, {0, 0}
};
const PoomNote SoundStageFinished[] PROGMEM = {
  {NoteG2, 100}, {NoteG3, 150}, {NoteG4, 350}, {0, 0}
};
const PoomNote SoundHit[] PROGMEM = {
  {NoteGs3, 25}, {NoteG3, 15}, {0, 0}
};
const PoomNote SoundPickupCoin[] PROGMEM = {
  {NoteCs6, 30}, {NoteCs5, 40}, {0, 0}
};
const PoomNote SoundPickupKnife[] PROGMEM = {
  {NoteCs6, 30}, {NoteCs7, 40}, {0, 0}
};

void Util::toggle(uint8_t & flags, uint8_t mask)
{
  if (flags & mask)
  {
    flags &= ~mask;
  }
  else
  {
    flags |= mask;
  }
}

bool Util::collideRect(int16_t x1, int8_t y1, uint8_t width1, uint8_t height1, int16_t x2, int8_t y2, uint8_t width2, uint8_t height2)
{

  return !(x1            >= x2 + width2  ||
           x1 + width1   <= x2           ||
           y1            >= y2 + height2 ||
           y1 + height1  <= y2);
}


// Inspired by TEAMArg's Sirene, stages.h:775
// But optimized (use of int8_t, use cast instead of for loop)
// Also use alignment (LEFT, RIGHT, CENTER) instead of zero padding
void Util::drawNumber(int16_t x, int16_t y, uint16_t value, uint8_t align)
{
  char buf[10];
  ltoa(value, buf, 10);
  uint8_t strLength = strlen(buf);
  int8_t offset = 0;
  switch (align)
  {
    case ALIGN_LEFT:
      offset = 0;
      break;
    case ALIGN_CENTER:
      offset = -(strLength * 2);
      break;
    case ALIGN_RIGHT:
      offset = -(strLength * 4);
      break;
  }

  // draw the frame
  Poom.fillRect(x + offset - 1, y, 4 * strLength + 1, 7, false);

  // draw the number
  for (uint8_t i = 0; i < strLength; i++)
  {
    uint8_t digit = (uint8_t) buf[i];
    digit -= 48;
    if (digit > 9) digit = 0;
    Poom.drawPageAsset(x + offset + 4 * i, y, font, digit);
  }
}

int freeRam()
{
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

#ifdef DEBUG_LOG
#include "menu.h"
int16_t debugValue = 0;
void drawDebugLog()
{
  Util::drawNumber(0, 0, debugValue, ALIGN_LEFT);
}
#endif

#ifdef DEBUG_CPU
#include "menu.h"
void drawDebugCpu()
{
  Util::drawNumber(128, 0, 0, ALIGN_RIGHT);
}
#endif

#ifdef DEBUG_RAM
#include "menu.h"
void drawDebugRam()
{
  extern int __heap_start, *__brkval;
  int v;
  int ram = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);

  Util::drawNumber(64, 0, ram, ALIGN_CENTER);
}
#endif
