#include "HollowPoom.h"

namespace {

PROGMEM const uint32_t hollowFont[] = {
  0x00000000, 0x00017000, 0x000C00C0, 0x0A7CA7CA, 0x0855F542, 0x19484253, 0x1251F55E, 0x00003000,
  0x00452700, 0x001C9440, 0x0519F314, 0x0411F104, 0x00000420, 0x04104104, 0x00000400, 0x01084210,
  0x0F45145E, 0x0001F040, 0x13555559, 0x0D5D5551, 0x087C928C, 0x0D555557, 0x0D55555E, 0x010C5251,
  0x0F55555E, 0x0F555556, 0x0000A000, 0x0000A400, 0x0028C200, 0x0028A280, 0x00086280, 0x000D5040,
  0x0018E300, 0x1F24929C, 0x0D5D555F, 0x1145149C, 0x0725145F, 0x1155555F, 0x0114515F, 0x1D55545E,
  0x1F10411F, 0x0045F440, 0x07210410, 0x1D18411F, 0x1041041F, 0x1F04F05E, 0x1F04109C, 0x0F45545E,
  0x0314925F, 0x1F45D45E, 0x1B34925F, 0x0D555556, 0x0105F041, 0x0721041F, 0x0108421F, 0x0F41E41F,
  0x1D184317, 0x0109C107, 0x114D5651, 0x0045F000, 0x0001F000, 0x0001F440, 0x000C1080, 0x10410410,
};

int16_t cursorX = 0;
int16_t cursorY = 0;
uint8_t textForeground = 1;
uint8_t textBackground = 0;

uint8_t scorePriority = 255;
bool scorePlaying = false;

void drawCharacter(int16_t x, int16_t y, unsigned char character)
{
  if (character < ' ' || character > '_') return;
  if (x >= PoomScreenWidth || y >= PoomScreenHeight || x + 5 < 0 || y + 6 < 0) return;
  uint32_t pattern = pgm_read_dword(hollowFont + (character - ' '));
  const bool drawBackground = textBackground != textForeground;
  for (int8_t column = 0; column < 6; ++column) {
    for (int8_t row = 0; row < 6; ++row) {
      const bool foreground = (pattern & 1U) != 0;
      if (foreground || drawBackground) {
        Poom.drawPixel(x + column, y + row,
          (foreground ? textForeground : textBackground) != 0);
      }
      pattern >>= 1;
    }
  }
}

size_t writeCharacter(uint8_t character)
{
  if (character == '\n') {
    cursorY += 6;
    cursorX = 0;
  } else if (character >= ' ' && character <= '_') {
    drawCharacter(cursorX, cursorY, character);
    cursorX += 6;
  }
  return 1;
}

} // namespace

namespace HollowPoom {

void setTextColor(uint8_t foreground, uint8_t background)
{
  textForeground = foreground;
  textBackground = background;
}

void setCursor(int16_t x, int16_t y)
{
  cursorX = x;
  cursorY = y;
}

size_t print(const char *text)
{
  if (text == nullptr) return 0;
  size_t written = 0;
  while (*text != '\0') written += writeCharacter(static_cast<uint8_t>(*text++));
  return written;
}

size_t print(const __FlashStringHelper *text)
{
  if (text == nullptr) return 0;
  PGM_P cursor = reinterpret_cast<PGM_P>(text);
  size_t written = 0;
  while (true) {
    const uint8_t character = pgm_read_byte(cursor++);
    if (character == 0) return written;
    written += writeCharacter(character);
  }
}

size_t print(int value)
{
  char buffer[16];
  return print(itoa(value, buffer, 10));
}

size_t print(unsigned int value)
{
  char buffer[16];
  return print(utoa(value, buffer, 10));
}

size_t print(long value)
{
  char buffer[24];
  return print(ltoa(value, buffer, 10));
}

size_t print(unsigned long value)
{
  char buffer[24];
  return print(ultoa(value, buffer, 10));
}

size_t printAt(int16_t x, int16_t y, const char *text)
{
  setCursor(x, y);
  return print(text);
}

size_t printAt(int16_t x, int16_t y, const __FlashStringHelper *text)
{
  setCursor(x, y);
  return print(text);
}

void playScore(const PoomNote *score, uint8_t priority)
{
  if (!Poom.audio().enabled() || score == nullptr) return;
  if (scorePlaying) {
    if (priority > scorePriority) return;
    stopScore();
  }
  scorePriority = priority;
  Poom.audio().play(score);
  scorePlaying = Poom.audio().isPlaying();
}

void stopScore()
{
  scorePlaying = false;
  scorePriority = 255;
  Poom.audio().stop();
}

void updateAudio()
{
  if (!scorePlaying) return;
  if (!Poom.audio().enabled() || !Poom.audio().isPlaying()) {
    scorePlaying = false;
    scorePriority = 255;
  }
}

} // namespace HollowPoom
