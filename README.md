# Poom

Poom is an Arduino library for developing applications on the Poom device.
It provides a clean ESP32-C5 foundation for bringing up the Poom display,
buttons, buzzer, and LEDs with a small game-friendly API.

## Installation

1. Copy or clone this repository into your Arduino libraries folder:
   `~/Arduino/libraries/poom-arduino`
2. Restart the Arduino IDE if it was already open.
3. Include the library in your sketch with:

```cpp
#include <Poom.h>
```

## Supported Hardware

- Poom hardware based on `ESP32-C5`

## Dependencies

This library depends on:

- `Adafruit SH110X`
- `Adafruit NeoPixel`

These dependencies are declared in `library.properties`. They should be
installed through the Arduino ecosystem, not copied into `src/`.

## Minimal Example

```cpp
#include <Poom.h>

void setup()
{
    Poom.begin();
    Poom.clear();
    Poom.setCursor(0, 0);
    Poom.println(F("Poom ready!"));
    Poom.show();
}

void loop()
{
}
```

## Available Modules

After calling `Poom.begin()`, the public API provides:

- direct methods like `Poom.clear()`, `Poom.setCursor()`, `Poom.print()`,
  `Poom.show()`, `Poom.drawBitmap()`, `Poom.drawBitmapMasked()`,
  `Poom.drawSprite()`, and `Poom.drawSpriteMasked()`, plus the explicitly named
  page-packed methods
  `Poom.drawPageBitmap()`, `Poom.drawPageBitmapMasked()`, and
  `Poom.drawPageSprite()`,
  `Poom.setFrameRate()`, and `Poom.nextFrame()`
- `Poom.graphics()` for row-packed and page-packed 1bpp bitmap and sprite helpers
- `Poom.buffer()` and `Poom.framebuffer()` for fast game rendering through
  Poom's 128x64 1bpp framebuffer
- `Poom.screen()` for advanced SH1106 access when needed
- `Poom.buttons()` for A/B and D-pad buttons
- `Poom.buzzer()` and `Poom.audio()` for simple tones and non-blocking
  `PoomNote` sequences
- `Poom.leds()` for the onboard WS2812 LEDs

`drawBitmap()` and `drawSprite()` continue to use row-packed data. Use the
`drawPage*()` methods for page-packed assets, where each byte represents eight
vertical pixels and bit 0 is the top pixel. Legacy assets that alternate an
image byte and mask byte can be split into two arrays offline:

```sh
python3 extras/convert_interleaved_page_sprite.py sprite.h \
    --width 16 --height 16 --name player -o player_page.h
```

The generated `playerFrames` and `playerMasks` arrays can be passed to
`drawPageBitmapMasked()` one frame at a time. Keeping conversion offline avoids
adding a legacy interleaved encoding to the runtime graphics API.

## Sound Sequences

Games can play short non-blocking sounds with Poom's own note format:

```cpp
const PoomNote pickup[] = {
    {880, 60},
    {0, 30},
    {1320, 90},
    {0, 0}
};

void setup()
{
    Poom.begin();
    Poom.audio().play(pickup);
}

void loop()
{
    Poom.update();
}
```

Use `{0, duration}` for a rest and `{0, 0}` to end the sequence.

## Examples

- `HelloPoom`: minimal OLED hello world
- `LedsDemo`: cycle through LED colors
- `BuzzerDemo`: play simple tones
- `ButtonsDemo`: show the pressed button on screen
- `SpriteDemo`: draw an animated bitmap sprite with Poom's native format

## License

Copyright (C) 2026 Poom.

This library is licensed under the GNU General Public License v3.0 or later.
See `LICENSE`.
