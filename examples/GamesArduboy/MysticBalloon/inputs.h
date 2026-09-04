#ifndef INPUT_H
#define INPUT_H

#include <Arduino.h>
#include "globals.h"
#include "player.h"

#define TIMER_AMOUNT 48

void checkInputs()
{
  if (kid.balloons <= 0)
    return; // Cannot control player if dead

    
  cam.offset = vec2(0, 0);
  kid.isWalking = false;
  if (Poom.anyPressed(PoomButtonDown))
  {
    cam.offset.y = -CAMERA_OFFSET;
  }
  else if (Poom.anyPressed(PoomButtonUp))
  {
    cam.offset.y = CAMERA_OFFSET;
  }
  if (!kid.isSucking) 
  {
    if (Poom.anyPressed(PoomButtonLeft))
    {
      mapTimer = TIMER_AMOUNT;
      cam.offset.x = CAMERA_OFFSET;
      kid.direction = FACING_LEFT;
      if (!(kid.isJumping || kid.isBalloon || kid.isLanding))
      {
        if (!gridGetSolid((kid.pos.x - 1) >> 4, (kid.pos.y + 8) >> 4))
          kid.actualpos.x -= PLAYER_SPEED_WALKING;
        kid.isWalking = true;
        kid.speed.x = -1;
      }
      else
      {
        //kid.speed.x = max(kid.speed.x - PLAYER_SPEED_AIR, -MAX_XSPEED);
        if (kid.speed.x > -MAX_XSPEED)
          kid.speed.x -= PLAYER_SPEED_AIR;
      }
    }
    else if (Poom.anyPressed(PoomButtonRight))
    {
      //mapTimer = TIMER_AMOUNT;
      cam.offset.x = -CAMERA_OFFSET;
      kid.direction = FACING_RIGHT;
      if (!(kid.isJumping || kid.isBalloon || kid.isLanding))
      {
        if (!gridGetSolid((kid.pos.x + 12) >> 4, (kid.pos.y + 8) >> 4))
          kid.actualpos.x += PLAYER_SPEED_WALKING;
        kid.isWalking = true;
        kid.speed.x = 1;
      }
      else
      {
        //kid.speed.x = min(kid.speed.x + PLAYER_SPEED_AIR, MAX_XSPEED);
        if (kid.speed.x < MAX_XSPEED)
          kid.speed.x += PLAYER_SPEED_AIR;
      }
    }
  }
  kid.isSucking = false;
  if (Poom.anyPressed(PoomButtonA))
  {
    if (Poom.anyPressed(PoomButtonDown))
      gameState = STATE_GAME_PAUSE;
    else //if (!kid.isBalloon)
    {
      kid.isBalloon = false;
      kid.isSucking = true;
    }
  }
  /*if (Poom.anyPressed(PoomButtonA + PoomButtonDown))  gameState = STATE_GAME_PAUSE;
  if (Poom.anyPressed(PoomButtonA) && !kid.isBalloon)
  {
    kid.isSucking = true;
  }
  else
    kid.isSucking = false;*/

  // Jump Button
  if (Poom.justPressed(PoomButtonB))
  {
    if (kid.speed.y == 0 && kid.isJumping == false && kid.isLanding == false)
    {
      Poom.audio().tone(200, 100);
      kid.isWalking = false;
      kid.isJumping = true;
      kid.jumpLetGo = false;
      kid.jumpTimer = PLAYER_JUMP_TIME;
      kid.speed.y = PLAYER_JUMP_VELOCITY;
      if (Poom.anyPressed(PoomButtonRight)) kid.speed.x = MAX_XSPEED;
      else if (Poom.anyPressed(PoomButtonLeft)) kid.speed.x = -MAX_XSPEED;
    }
    else
    {
      if (kid.balloons > 0)
      {
        kid.isBalloon = true;
        kid.balloonOffset = 16;
        kid.isJumping = false;
        kid.isLanding = true;
      }
    }
  }
  if (!Poom.anyPressed(PoomButtonB))
  {
    kid.isBalloon = false;
    if (kid.isJumping) kid.jumpLetGo = true;
  }
}

#endif
