#ifndef GAME_H
#define GAME_H

#include <Arduino.h>
#include "globals.h"
#include "inputs.h"
#include "player.h"
#include "enemies.h"
#include "elements.h"
#include "levels.h"

#define TOTAL_TONES 10
PROGMEM const byte tones[] = {
  //200, 100, 250, 125, 300, 150, 350, 400, 425, 475
  131, 145, 139, 152, 131, 172, 200, 188, 213, 255
};

byte toneindex = 0;

void stateMenuPlayNew()
{
  level = LEVEL_TO_START_WITH - 1;
  coinsCollected = 0;
  totalCoins = 0;
  balloonsLeft = 0;
  scorePlayer = 0;
  globalCounter = 0;
  kid.balloons = 3;
  gameState = STATE_GAME_NEXT_LEVEL;
  scoreIsVisible = false;
  nextLevelIsVisible = true;
  pressKeyIsVisible = false;
  progressSavePending = true;
  resetNewGameProgress();
}

void stateMenuPlayContinue()
{
  loadProgress();
  coinsCollected = 0;
  balloonsLeft = 0;
  globalCounter = 0;
  kid.balloons = 3;
  gameState = STATE_GAME_NEXT_LEVEL;
  scoreIsVisible = false;
  nextLevelIsVisible = true;
  pressKeyIsVisible = false;
  progressSavePending = true;
}


void stateGameNextLevel()
{
  //if (level < TOTAL_LEVELS)
  //{
    if (Poom.everyFrames(20))
    {
      canPressButton = false;
      if (coinsCollected > 0)
      {
        coinsCollected--;
        scorePlayer += 20;
        Poom.audio().tone(pgm_read_byte(tones + toneindex++), 150);
      }
      else if (balloonsLeft > 0)
      {
        balloonsLeft--;
        scorePlayer += 30;
        Poom.audio().tone(pgm_read_byte(tones + toneindex++), 150);
      }
      else
      {
        canPressButton = true;
        scoreIsVisible = false;
        pressKeyIsVisible = !pressKeyIsVisible;
        if (toneindex < TOTAL_TONES)
        {
          Poom.audio().tone(pgm_read_byte(tones + toneindex++), 200);
          toneindex = TOTAL_TONES;
        }
        if (level >= TOTAL_LEVELS)
          gameState = STATE_GAME_OVER;

        if (progressSavePending)
        {
          if (level < TOTAL_LEVELS)
          {
            saveProgress();
          }
          else
          {
            resetCompletedRunProgress();
          }
          progressSavePending = false;
        }
      }
    }
  /*}
  else
  {
    gameState = STATE_GAME_OVER;
    return;
  }*/

  //if (nextLevelIsVisible)
  //{
  if (level < TOTAL_LEVELS)
  {
    Poom.drawPageAsset(35, 4, badgeNextLevel, 0);
    drawNumbers(78, 13, FONT_BIG, DATA_LEVEL);
  }
  else
  {
    // The completed-run reset is committed above once tallying has finished.
  }
  drawNumbers(43, 49, FONT_BIG, DATA_SCORE);
  //}

  if (scoreIsVisible)
  {
    byte totalBadges = coinsCollected + balloonsLeft;

    for (byte i = 0; i < totalBadges; ++i)
    {
      if (i < coinsCollected) Poom.overwritePageAsset(65 - (7 * totalBadges) + (i * 14), 27, badgeElements, 0);
      else Poom.overwritePageAsset(65 - (7 * totalBadges) + (i * 14), 27, badgeElements, 1);
    }
  }

  if (canPressButton)
  {

    if (pressKeyIsVisible) Poom.overwritePageAsset(38, 29, badgePressKey, 0);
    if (Poom.justPressed(PoomButtonA | PoomButtonB))
    {
      toneindex = 0;
      Poom.audio().tone(425, 20);
      setKid();
      //cam.pos = vec2(0, 0);
      cam.pos = vec2(0, LEVEL_HEIGHT - 64);
      cam.offset = vec2(0, 0);
      enemiesInit();
      levelLoad(levels[level]);
      gameState = STATE_GAME_PLAYING;
    }
  }
};


void stateGamePlaying()
{
  checkInputs();
  checkKid();
  updateCamera();

  drawGrid();
  enemiesUpdate();

  drawKid();
  drawHUD();

  checkCollisions();
}


void stateGamePause()
{
  Poom.drawPageAsset(47, 17, badgePause, 0);
  if (Poom.justPressed(PoomButtonA | PoomButtonB))
  {
    gameState = STATE_GAME_PLAYING;
  }
}


void stateGameOver()
{
  byte x = 35 + 12;
  if (level < TOTAL_LEVELS)
  {
    drawNumbers(78, 26, FONT_BIG, DATA_LEVEL);
    x -= 12;
  }
  Poom.drawPageAsset(x, 17, badgeGameOver, 0);
  drawNumbers(43, 49, FONT_BIG, DATA_SCORE);

  const uint32_t highscore = readHighScore();
  if (scorePlayer > highscore) {
    saveHighScore();
  }
    
  if (Poom.justPressed(PoomButtonA | PoomButtonB))
  {
    gameState = STATE_MENU_MAIN;
  }
}

#endif
