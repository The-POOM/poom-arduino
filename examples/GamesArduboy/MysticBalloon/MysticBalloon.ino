/*
  Mystic Balloon: http://www.team-arg.org/mybl-manual.html

  Arduboy version 1.7.2:  http://www.team-arg.org/mybl-downloads.html

  MADE by TEAM a.r.g. : http://www.team-arg.org/more-about.html

  2016-2018 - GAVENO - CastPixel - JO3RI - Martian220

  Game License: MIT : https://opensource.org/licenses/MIT

*/

//determine the game
#define GAME_ID 34

#include "globals.h"
#include "menu.h"
#include "game.h"
#include "inputs.h"
#include "player.h"
#include "enemies.h"
#include "elements.h"
#include "levels.h"


typedef void (*FunctionPointer) ();

const FunctionPointer mainGameLoop[] = {
  stateMenuIntro,
  stateMenuMain,
  stateMenuHelp,
  stateMenuPlaySelect,
  stateMenuInfo,
  stateMenuSoundfx,
  stateGameNextLevel,
  stateGamePlaying,
  stateGamePause,
  stateGameOver,
  stateMenuPlayContinue,
  stateMenuPlayNew,
};

constexpr uint8_t MysticUpdateRate = 60;
constexpr uint8_t MysticFallbackPresentationRate = 45;

uint16_t presentationAccumulator = 0;
uint16_t slowPresentationTicks = 0;

bool shouldPresentFrame()
{
  const bool usingFallbackRate = slowPresentationTicks != 0;
  if (usingFallbackRate) --slowPresentationTicks;

  const uint8_t presentationRate = usingFallbackRate
    ? MysticFallbackPresentationRate
    : MysticUpdateRate;
  presentationAccumulator += presentationRate;
  if (presentationAccumulator < MysticUpdateRate) return false;

  presentationAccumulator -= MysticUpdateRate;
  return true;
}

void recordFrameCost(uint32_t frameWorkMicros)
{
  const uint32_t frameBudgetMicros = 1000000UL / MysticUpdateRate;
  if (frameWorkMicros > frameBudgetMicros)
  {
    slowPresentationTicks = MysticUpdateRate;
  }
}

void setup()
{
  Poom.begin();
  Poom.audio().loadEnabled();
  Poom.setFrameRate(MysticUpdateRate);
  loadSetStorage();
}

void loop() {
  if (!Poom.nextFrame()) return;

  const uint32_t frameWorkStartedMicros = micros();
  if (gameState < STATE_GAME_NEXT_LEVEL && Poom.everyFrames(10)) sparkleFrames = (sparkleFrames + 1) % 5;
  Poom.fill(false);
  mainGameLoop[gameState]();

  if (shouldPresentFrame()) Poom.show();
  recordFrameCost(micros() - frameWorkStartedMicros);
}
