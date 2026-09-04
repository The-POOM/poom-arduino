#include <Poom.h>
#include "Game.h"
#include "Draw.h"
#include "FixedMath.h"
#include "Platform.h"

uint8_t Platform::GetInput()
{
  uint8_t result = 0;
  
  if(Poom.pressed(PoomButtonA))
  {
    result |= INPUT_A;  
  }
  if(Poom.pressed(PoomButtonB))
  {
    result |= INPUT_B;  
  }
  if(Poom.pressed(PoomButtonUp))
  {
    result |= INPUT_UP;  
  }
  if(Poom.pressed(PoomButtonDown))
  {
    result |= INPUT_DOWN;  
  }
  if(Poom.pressed(PoomButtonLeft))
  {
    result |= INPUT_LEFT;  
  }
  if(Poom.pressed(PoomButtonRight))
  {
    result |= INPUT_RIGHT;  
  }

  return result;
}

void Platform::PlaySound(const PoomNote* audioPattern)
{
	Poom.audio().play(audioPattern);
}

void Platform::SetLED(uint8_t r, uint8_t g, uint8_t b)
{
  Poom.leds().setColor(r ? 255 : 0, g ? 255 : 0, b ? 255 : 0);
}

void Platform::PutPixel(uint8_t x, uint8_t y, uint8_t colour)
{
  Poom.drawPixel(x, y, colour != 0);
}

// Adpated from https://github.com/a1k0n/arduboy3d/blob/master/draw.cpp
// since the AVR has no barrel shifter, we'll do a progmem lookup
const uint8_t topmask_[] PROGMEM = {
  0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80 };
const uint8_t bottommask_[] PROGMEM = {
  0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff };

void Platform::DrawVLine(uint8_t x, int8_t y0_, int8_t y1_, uint8_t pattern) 
{
  uint8_t *screenptr = Poom.buffer() + x;

  if (y1_ < y0_ || y1_ < 0 || y0_ > 63) return;

  // Clip to the viewport, then use unsigned coordinates below.
  uint8_t y0 = y0_, y1 = y1_;
  if (y0_ < 0) y0 = 0;
  if (y1_ > 63) y1 = 63;

  uint8_t *page0 = screenptr + ((y0 & 0x38) << 4);
  uint8_t *page1 = screenptr + ((y1 & 0x38) << 4);
  if (page0 == page1) 
  {
    uint8_t mask = pgm_read_byte(topmask_ + (y0 & 7))
      & pgm_read_byte(bottommask_ + (y1 & 7));
    *page0 &= ~mask;
    *page0 |= pattern & mask;  // fill y0..y1 in same page in one shot
  }
  else
  {
    uint8_t mask = pgm_read_byte(topmask_ + (y0 & 7));
    *page0 &= ~mask;
    *page0 |= pattern & mask;  // write top 1..8 pixels
    page0 += 128;
    while (page0 != page1) 
    {
      *page0 = pattern;  // fill middle 8 pixels at a time
      page0 += 128;
    }
    mask = pgm_read_byte(bottommask_ + (y1 & 7));  // and bottom 1..8 pixels
    *page0 &= ~mask;
    *page0 |= pattern & mask;
  }
}

uint8_t* Platform::GetScreenBuffer()
{
  return Poom.buffer();
}

void Platform::DrawSprite(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t frame)
{
  Poom.drawPageAssetInterleavedMasked(x, y, bitmap, frame);
}

void Platform::DrawSprite(int16_t x, int16_t y, const uint8_t *bitmap,
  const uint8_t *mask, uint8_t frame, uint8_t mask_frame)
{
  Poom.drawPageAssetMasked(x, y, bitmap, mask, frame, mask_frame);
}

void Platform::DrawBitmap(int16_t x, int16_t y, const uint8_t *bitmap)
{
  uint8_t w = pgm_read_byte(&bitmap[0]);
  uint8_t h = pgm_read_byte(&bitmap[1]);
  Poom.drawPageBitmap(x, y, bitmap + 2, w, h);
}

void Platform::DrawSolidBitmap(int16_t x, int16_t y, const uint8_t *bitmap)
{
  uint8_t w = pgm_read_byte(&bitmap[0]);
  uint8_t h = pgm_read_byte(&bitmap[1]);
  Poom.fillRect(x, y, w, h, false);
  Poom.drawPageBitmap(x, y, bitmap + 2, w, h);
}

void Platform::FillScreen(uint8_t colour)
{
  Poom.fill(colour != 0);
}

uint16_t gameTickAccumulator = 0;
uint16_t presentationAccumulator = 0;
uint16_t slowPresentationTicks = 0;

struct RenderPose
{
  int16_t x;
  int16_t y;
  uint8_t angle;
};

RenderPose previousRenderPose = {};
RenderPose currentRenderPose = {};

void CaptureCurrentRenderPose(RenderPose &pose)
{
  pose.x = Game::player.x;
  pose.y = Game::player.y;
  pose.angle = Game::player.angle;
}

void ResetRenderInterpolation()
{
  CaptureCurrentRenderPose(previousRenderPose);
  currentRenderPose = previousRenderPose;
}

int16_t InterpolateCoordinate(int16_t from, int16_t to, uint16_t phase)
{
  return static_cast<int16_t>(
    from + (static_cast<int32_t>(to - from) * phase) / TARGET_FRAMERATE
  );
}

uint8_t InterpolateAngle(uint8_t from, uint8_t to, uint16_t phase)
{
  const int8_t shortestDelta = static_cast<int8_t>(to - from);
  return static_cast<uint8_t>(
    from + (static_cast<int16_t>(shortestDelta) * phase) / TARGET_FRAMERATE
  );
}

void DrawInterpolatedGame()
{
  const int16_t cameraX = InterpolateCoordinate(
    previousRenderPose.x,
    currentRenderPose.x,
    gameTickAccumulator
  );
  const int16_t cameraY = InterpolateCoordinate(
    previousRenderPose.y,
    currentRenderPose.y,
    gameTickAccumulator
  );
  const uint8_t cameraAngle = InterpolateAngle(
    previousRenderPose.angle,
    currentRenderPose.angle,
    gameTickAccumulator
  );
  Game::Draw(cameraX, cameraY, cameraAngle);
}

bool ShouldPresentFrame()
{
  const bool usingFallbackRate = slowPresentationTicks != 0;
  if (usingFallbackRate)
  {
    slowPresentationTicks--;
  }

  const uint8_t presentationRate = usingFallbackRate
    ? FALLBACK_PRESENTATION_FRAMERATE
    : TARGET_FRAMERATE;
  presentationAccumulator += presentationRate;
  if (presentationAccumulator < TARGET_FRAMERATE)
  {
    return false;
  }

  presentationAccumulator -= TARGET_FRAMERATE;
  return true;
}

void UpdateGameAtOriginalRate()
{
  gameTickAccumulator += GAME_TICK_FRAMERATE;
  while (gameTickAccumulator >= TARGET_FRAMERATE)
  {
    gameTickAccumulator -= TARGET_FRAMERATE;
    previousRenderPose = currentRenderPose;
    Game::Tick();
    CaptureCurrentRenderPose(currentRenderPose);
  }
}

void RecordFrameCost(uint32_t frameWorkMicros)
{
  const uint32_t frameBudgetMicros = 1000000UL / TARGET_FRAMERATE;
  if (frameWorkMicros > frameBudgetMicros)
  {
    slowPresentationTicks = TARGET_FRAMERATE;
  }
}

bool Platform::IsAudioEnabled()
{
	return Poom.audio().enabled();
}

void Platform::SetAudioEnabled(bool isEnabled)
{
	Poom.audio().setEnabled(isEnabled);
	Poom.audio().saveEnabled();
}

void Platform::ExpectLoadDelay()
{
	// Discard the fractional simulation step after a long level-generation pass.
	gameTickAccumulator = 0;
	ResetRenderInterpolation();
}

void setup()
{
  Poom.begin();
  Poom.audio().loadEnabled();
  Poom.setFrameRate(TARGET_FRAMERATE);

//  SeedRandom((uint16_t)(micros() ^ millis()));
  Game::Init();
  ResetRenderInterpolation();
}

void loop()
{
  if(Poom.nextFrame())
  {
	UpdateGameAtOriginalRate();

	const uint32_t frameWorkStartedMicros = micros();
	DrawInterpolatedGame();
    
#if DEV_MODE
	// CPU load bar graph	
	int load = 0;
	uint8_t* screenPtr = Poom.buffer();
	
	for(int x = 0; x < load && x < 128; x++)
	{
		screenPtr[x] = (screenPtr[x] & 0xf8) | 3;
	}
	screenPtr[100] = 0;
#endif
	
    if (ShouldPresentFrame())
    {
      Poom.show();
    }
    RecordFrameCost(micros() - frameWorkStartedMicros);
  }
}
