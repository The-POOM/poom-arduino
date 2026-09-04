#include "global.h"

#include "menu.h"
#include "game.h"
#include "assets.h"

uint8_t bootCounter = 0;

constexpr uint8_t CastleFallbackPresentationRate = 45;

uint16_t presentationAccumulator = 0;
uint16_t slowPresentationTicks = 0;

bool shouldPresentFrame()
{
  const bool usingFallbackRate = slowPresentationTicks != 0;
  if (usingFallbackRate) --slowPresentationTicks;

  const uint8_t presentationRate = usingFallbackRate
      ? CastleFallbackPresentationRate
      : FPS;
  presentationAccumulator += presentationRate;
  if (presentationAccumulator < FPS) return false;

  presentationAccumulator -= FPS;
  return true;
}

void recordFrameCost(uint32_t frameWorkMicros)
{
  const uint32_t frameBudgetMicros = 1000000UL / FPS;
  if (frameWorkMicros > frameBudgetMicros)
  {
    slowPresentationTicks = FPS;
  }
}

void setup()
{
  Poom.begin();
  Poom.audio().loadEnabled();
  Poom.setFrameRate(FPS);

  Menu::showTitle();
}

void loop()
{
  if (!Poom.nextFrame())
  {
    return;
  }

  const uint32_t frameWorkStartedMicros = micros();
  Poom.fill(false);
  
  if(bootCounter < 120)
  {
    bootCounter++;
    Poom.overwritePageAsset(49, 14, logo, 0);
  }
  else
  {
    Menu::loop();

#ifdef DEBUG_LOG
    drawDebugLog();
#endif

#ifdef DEBUG_CPU
    drawDebugCpu();
#endif

#ifdef DEBUG_RAM
    drawDebugRam();
#endif

    if (flashCounter > 0)
    {
      Poom.fillRect(0, 0, 128, 64);
      flashCounter--;
    }
  }

  if (shouldPresentFrame()) Poom.show();
  recordFrameCost(micros() - frameWorkStartedMicros);
}
