#ifndef GLOBALS_H
#define GLOBALS_H

/*-----------------------------*
   To turn on hard mode
   uncomment the below define.

   Hard mode makes it so you
   start each level without
   recovering balloons.
  ----------------------------*/
//#define HARD_MODE

#include <Arduino.h>
#include <Poom.h>
#include "vec2.h"
#include "bitmaps.h"

//define menu states (on main menu)
#define STATE_MENU_INTRO             0
#define STATE_MENU_MAIN              1
#define STATE_MENU_HELP              2
#define STATE_MENU_PLAY              3
#define STATE_MENU_INFO              4
#define STATE_MENU_SOUNDFX           5

//define game states (on main menu)
#define STATE_GAME_NEXT_LEVEL        6
#define STATE_GAME_PLAYING           7
#define STATE_GAME_PAUSE             8
#define STATE_GAME_OVER              9
#define STATE_GAME_PLAYCONTNEW       10 // 11

#define FACING_RIGHT                 0
#define FACING_LEFT                  1

#define LEVEL_TO_START_WITH          1
#define TOTAL_LEVELS                 39
#define TOTAL_COINS                  TOTAL_LEVELS * 6

#define MAX_PER_TYPE                 6                    // total instances per enemy type

#define LEVEL_WIDTH                  384                  // 24 * 16
#define LEVEL_HEIGHT                 384                  // 24 * 16
#define LEVEL_CELLSIZE               16
#define LEVEL_WIDTH_CELLS            24
#define LEVEL_HEIGHT_CELLS           24
#define LEVEL_CELL_BYTES             (LEVEL_WIDTH_CELLS * LEVEL_HEIGHT_CELLS) >> 3
#define LEVEL_ARRAY_SIZE             576

#define PLAYER_JUMP_TIME             11

// This is a replacement for struct Rect in the Arduboy2 library.
// It defines height as an int instead of a uint8_t to allow a higher rectangle.
struct HighRect
{
  public:
    int x;
    int y;
    uint16_t width;
    int height;
};

byte gameState = STATE_MENU_INTRO;   // start the game with the TEAM a.r.g. logo
byte menuSelection = STATE_MENU_PLAY; // PLAY menu item is pre-selected
byte globalCounter = 0;
byte level;
unsigned long scorePlayer;
byte coinsCollected = 0;
byte totalCoins = 0;
byte balloonsLeft;

boolean nextLevelIsVisible;
boolean scoreIsVisible;
boolean canPressButton;
boolean pressKeyIsVisible;
boolean progressSavePending;

byte walkerFrame = 0;
byte fanFrame = 0;
byte coinFrame = 0;
byte coinsActive = 0;
vec2 levelExit = vec2(0, 0);
vec2 startPos;
byte mapTimer = 10;

void loadSetStorage()
{
  if (!Poom.storage().begin("mybl")) return;

  if (Poom.storage().readByte("version") != GAME_ID)
  {
    Poom.storage().writeByte("level", LEVEL_TO_START_WITH - 1);
    Poom.storage().writeByte("coins", 0);
    Poom.storage().writeByte("coins_high", 0);
    Poom.storage().writeUInt32("score", 0);
    Poom.storage().writeUInt32("score_high", 0);
    Poom.storage().writeByte("version", GAME_ID);
  }
}

void saveProgress()
{
  Poom.storage().writeByte("level", level);
  Poom.storage().writeByte("coins", totalCoins);
  Poom.storage().writeUInt32("score", scorePlayer);
}

void loadProgress()
{
  level = Poom.storage().readByte("level", LEVEL_TO_START_WITH - 1);
  totalCoins = Poom.storage().readByte("coins", 0);
  scorePlayer = Poom.storage().readUInt32("score", 0);
}

void resetNewGameProgress()
{
  Poom.storage().writeByte("level", LEVEL_TO_START_WITH - 1);
  Poom.storage().writeByte("coins", 0);
  Poom.storage().writeUInt32("score", 0);
}

void resetCompletedRunProgress()
{
  Poom.storage().writeByte("level", LEVEL_TO_START_WITH - 1);
  Poom.storage().writeUInt32("score", 0);
}

uint32_t readHighScore()
{
  return Poom.storage().readUInt32("score_high", 0);
}

uint8_t readHighScoreCoins()
{
  return Poom.storage().readByte("coins_high", 0);
}

void saveHighScore()
{
  Poom.storage().writeByte("coins_high", totalCoins);
  Poom.storage().writeUInt32("score_high", scorePlayer);
}

// This is a replacement for the collide() function in the Arduboy2 library.
// It uses struct HighRect instead of the struct Rect in the library.
bool collide(HighRect rect1, HighRect rect2)
{
  return !( rect2.x                 >=  rect1.x + rect1.width    ||
            rect2.x + rect2.width   <=  rect1.x                ||
            rect2.y                 >=  rect1.y + rect1.height ||
            rect2.y + rect2.height  <=  rect1.y);
}

#endif
