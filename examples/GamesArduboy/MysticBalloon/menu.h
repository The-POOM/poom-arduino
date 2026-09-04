#ifndef MENU_BITMAPS_H
#define MENU_BITMAPS_H

#include <Arduino.h>
#include "globals.h"

#define FONT_TINY                 0
#define FONT_SMALL                1
#define FONT_BIG                  2

#define DATA_TIMER                0
#define DATA_SCORE                1
#define DATA_LEVEL                2

byte blinkingFrames = 0;
byte sparkleFrames = 0;
byte cont = 0;

extern void drawNumbers(byte numbersX, byte numbersY, byte fontType, byte data);

void drawTitleScreen()
{
  if (Poom.everyFrames(8)) blinkingFrames = (blinkingFrames + 1) % 32;
  for (byte i = 0; i < 4; i++) Poom.drawPageAsset(32 * i, 0, titleScreen, i);
  Poom.drawPageAsset(85, 45, badgeMysticBalloon, 0);
  Poom.drawPageAsset(79, 43, stars, sparkleFrames);
  Poom.drawPageAsset(9, 9, leftGuyLeftEye, pgm_read_byte(&blinkingEyesLeftGuy[blinkingFrames]));
  Poom.drawPageAsset(15, 13, leftGuyRightEye, pgm_read_byte(&blinkingEyesLeftGuy[blinkingFrames]));
  Poom.drawPageAsset(109, 34, rightGuyEyes, pgm_read_byte(&blinkingEyesRightGuy[blinkingFrames]));
}

void stateMenuIntro()
{
  globalCounter++;
  if (globalCounter < 160)
  {
    Poom.drawPageAsset(34, 4, T_arg, 0);
  }
  else
  {
    drawTitleScreen();
    if ((globalCounter > 250) || Poom.justPressed(PoomButtonA | PoomButtonB))
    {
      gameState = STATE_MENU_MAIN;
      Poom.audio().tone(425, 20);
    }
  }
}

void stateMenuMain()
{

  drawTitleScreen();
  Poom.overwritePageAsset(51, 9, mainMenu, 0);
  if (Poom.justPressed(PoomButtonDown) && (menuSelection < 5))
  {
    menuSelection++;
    Poom.audio().tone(300, 20);
  }
  if (Poom.justPressed(PoomButtonUp) && (menuSelection > 2))
  {
    menuSelection--;
    Poom.audio().tone(300, 20);
  }
  if (Poom.justPressed(PoomButtonA | PoomButtonB))
  {
    gameState = menuSelection;
    Poom.audio().tone(425, 20);
  }
  Poom.drawPageAssetInterleavedMasked(46, 9 + 9 * (menuSelection - 2), selector_plus_mask, 0);
}

void stateMenuHelp()
{
  if (Poom.justPressed(PoomButtonA | PoomButtonB))
  {
    gameState = STATE_MENU_MAIN;
    Poom.audio().tone(425, 20);
  }
}


void stateMenuInfo()
{
  Poom.drawPageAsset(43, 2, badgeMysticBalloon, 0);
  Poom.drawPageAsset(37, 0, stars, sparkleFrames);
  Poom.drawPageAsset(40, 48, madeBy, 0);
  scorePlayer = readHighScore();
  if (readHighScoreCoins() == TOTAL_COINS)
  {
    Poom.drawPageAsset(21, 28, badgeSuper, 0);
  }
  else
  {
    Poom.drawPageAsset(28, 28, badgeBorder, 0);
  }
  Poom.drawPageAsset(30, 28, badgeHighScore, 0);
  drawNumbers(55, 30, FONT_BIG, DATA_SCORE);
  if (Poom.justPressed(PoomButtonA | PoomButtonB))
  {
    gameState = STATE_MENU_MAIN;
    Poom.audio().tone(425, 20);
  }
}

void stateMenuSoundfx()
{
  drawTitleScreen();
  Poom.overwritePageAsset(51, 9, soundMenu, 0);
  if (Poom.justPressed(PoomButtonDown))
  {
    Poom.audio().setEnabled(true);
    Poom.audio().tone(300, 20);
  }
  if (Poom.justPressed(PoomButtonUp)) Poom.audio().setEnabled(false);
  Poom.drawPageAssetInterleavedMasked(54, 18 + 9 * Poom.audio().enabled(), selector_plus_mask, 0);
  if (Poom.justPressed(PoomButtonA | PoomButtonB))
  {
    Poom.audio().saveEnabled();
    gameState = STATE_MENU_MAIN;
    Poom.audio().tone(425, 20);
  }
}

void stateMenuPlaySelect()
{
  drawTitleScreen();
  Poom.overwritePageAsset(53, 18, continueMenu, 0);
  if (Poom.justPressed(PoomButtonDown))
  {
    cont = 1;
    Poom.audio().tone(300, 20);
  }
  if (Poom.justPressed(PoomButtonUp))
  {
    cont = 0;
    Poom.audio().tone(300, 20);
  }
  Poom.drawPageAssetInterleavedMasked(48, 18 + 9 * cont, selector_plus_mask, 0);
  if (Poom.justPressed(PoomButtonB))
  {
    gameState = STATE_GAME_PLAYCONTNEW + cont;
    cont = 0;
    Poom.audio().tone(425, 20);
  }
  if (Poom.justPressed(PoomButtonA))
  {
    gameState = STATE_MENU_MAIN;
    Poom.audio().tone(425, 20);
  }
}


#endif
