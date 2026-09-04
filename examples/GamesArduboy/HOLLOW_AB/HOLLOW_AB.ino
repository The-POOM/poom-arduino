#include "common.h"

/*  Defines  */

enum MODE {
    LOGO_MODE = 0,
    TITLE_MODE,
    GAME_MODE
};

/*  Typedefs  */

typedef struct {
    void(*initFunc)(void);
    bool(*updateFunc)(void);
    void(*drawFunc)(void);
} MODULE_FUNCS;

/*  Local Variables  */

static const MODULE_FUNCS moduleTable[] = {
    { initLogo,  updateLogo,  drawLogo  }, 
    { initTitle, updateTitle, drawTitle }, 
    { initGame,  updateGame,  drawGame  }, 
};

static MODE mode = LOGO_MODE;

constexpr uint8_t HollowUpdateRate = 60;
constexpr uint8_t HollowFallbackPresentationRate = 45;

static uint16_t presentationAccumulator = 0;
static uint16_t slowPresentationTicks = 0;

/*---------------------------------------------------------------------------*/

static bool shouldPresentFrame()
{
    const bool usingFallbackRate = slowPresentationTicks != 0;
    if (usingFallbackRate) --slowPresentationTicks;

    const uint8_t presentationRate = usingFallbackRate
            ? HollowFallbackPresentationRate
            : HollowUpdateRate;
    presentationAccumulator += presentationRate;
    if (presentationAccumulator < HollowUpdateRate) return false;

    presentationAccumulator -= HollowUpdateRate;
    return true;
}

static void recordFrameCost(uint32_t frameWorkMicros)
{
    const uint32_t frameBudgetMicros = 1000000UL / HollowUpdateRate;
    if (frameWorkMicros > frameBudgetMicros) {
        slowPresentationTicks = HollowUpdateRate;
    }
}

void setup()
{
    Poom.begin();
    Poom.storage().begin("hollow");
    Poom.audio().loadEnabled();
    Poom.setFrameRate(HollowUpdateRate);
    moduleTable[LOGO_MODE].initFunc();
}

void loop()
{
    if (!Poom.nextFrame()) return;

    const uint32_t frameWorkStartedMicros = micros();
    HollowPoom::updateAudio();
    bool isDone = moduleTable[mode].updateFunc();
    moduleTable[mode].drawFunc();
    if (shouldPresentFrame()) Poom.show();
    recordFrameCost(micros() - frameWorkStartedMicros);

    if (isDone) {
        mode = (mode == TITLE_MODE) ? GAME_MODE : TITLE_MODE;
        moduleTable[mode].initFunc();
    }
}
