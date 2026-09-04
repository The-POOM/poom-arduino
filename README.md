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
  `Poom.drawSprite()`, and `Poom.drawSpriteMasked()`
- drawing primitives through `Poom.drawLine()`, `Poom.drawCircle()`,
  `Poom.fillCircle()`, `Poom.drawTriangle()`, `Poom.fillTriangle()`,
  `Poom.drawRoundRect()`, and `Poom.fillRoundRect()`
- Arduboy-compatible compressed art through `Poom.drawCompressed()` and
  `Poom.drawCompressedMirror()`
- explicitly named page-packed methods
  `Poom.drawPageBitmap()`, `Poom.drawPageBitmapMasked()`,
  `Poom.drawPageSprite()`, `Poom.drawPageSpriteMasked()`,
  `Poom.drawPageSpriteInterleavedMasked()`, `Poom.erasePageSprite()`, and
  `Poom.overwritePageSprite()`
- headered page assets through `Poom.drawPageAsset()`,
  `Poom.drawPageAssetInterleavedMasked()`, `Poom.erasePageAsset()`, and
  `Poom.overwritePageAsset()`
- frame scheduling through `Poom.setFrameRate()`, `Poom.nextFrame()`,
  `Poom.frameCount()`, and `Poom.everyFrames()`
- `Poom.graphics()` for row-packed and page-packed 1bpp bitmap and sprite helpers
- `Poom.buffer()` and `Poom.framebuffer()` for fast game rendering through
  Poom's 128x64 1bpp framebuffer
- `Poom.screen()` for advanced SH1106 access when needed
- `Poom.buttons()` for A/B and D-pad buttons
- `Poom.buzzer()` and `Poom.audio()` for simple tones and non-blocking
  `PoomNote` sequences
- `Poom.storage()` for game-specific, namespaced ESP32 Preferences data
- `PoomIMU` for opt-in LSM6DS3TR-C acceleration, rotation, and temperature
- `PoomMultiplayer` for opt-in two-player ESP-NOW communication
- `Poom.leds()` for the onboard WS2812 LEDs

`drawBitmap()` and `drawSprite()` continue to use row-packed data. Use the
`drawPage*()` methods for page-packed assets, where each byte represents eight
vertical pixels and bit 0 is the top pixel. Page drawing is performed directly
on whole framebuffer bytes, including sprites shifted between display pages.

Assets beginning with `width, height` can be drawn without a local decoder:

```cpp
Poom.drawPageAsset(x, y, player, frame);
Poom.drawPageAssetMasked(x, y, player, playerMask, frame, maskFrame);
Poom.drawPageAssetInterleavedMasked(x, y, maskedPlayer, frame);
Poom.erasePageAsset(x, y, particles, frame);
Poom.overwritePageAsset(x, y, tiles, frame);
```

Interleaved image/mask assets are supported directly. They can also be split
into separate arrays offline when that layout is preferable:

```sh
python3 extras/convert_interleaved_page_sprite.py sprite.h \
    --width 16 --height 16 --name player -o player_page.h
```

The generated `playerFrames` and `playerMasks` arrays can be passed to
`drawPageSpriteMasked()`.

Standard Arduboy `drawCompressed()` arrays can be reused without converting
them. Set pixels are layered over the existing framebuffer. Pass `false` to
clear those pixels instead, which is useful for the mask-first drawing pattern:

```cpp
Poom.drawCompressed(x, y, playerMask, false);
Poom.drawCompressed(x, y, playerImage);
Poom.drawCompressedMirror(x, y, playerImage, true, true);
```

`Poom.show()` sends the 1 KB page framebuffer to the SH1106 as page-sized I2C
data instead of converting all 8,192 pixels through Adafruit GFX. The transfer
uses the SH1106 column offset and page commands used by POOM firmware. Nonzero
custom display rotations retain a slower pixel-transform fallback so their
orientation remains correct.

After the first full frame, POOM compares each page against a 1 KB shadow and
only transmits changed column ranges. The OLED uses an 800 kHz I2C clock by
default. The IMU reselects its supported 400 kHz clock before its own
transactions, and timing/FPS counters are available at runtime:

```cpp
const PoomDisplayMetrics &stats = Poom.screen().metrics();
Serial.printf("display=%u fps, last=%lu us, bytes=%u, errors=%lu\n",
              stats.presentFps,
              static_cast<unsigned long>(stats.lastPresentMicros),
              stats.lastDataBytes,
              static_cast<unsigned long>(stats.errorCount));
```

The default is `POOM_DISPLAY_I2C_CLOCK_HZ` (800 kHz). It can be overridden at
build time or changed at runtime with `setI2CClock()` for other board revisions.

## Frame Scheduling

`nextFrame()` increments a POOM-owned frame counter only when it returns true.
Use `everyFrames()` for periodic game logic:

```cpp
if (!Poom.nextFrame()) return;
if (Poom.everyFrames(8)) animationFrame = (animationFrame + 1) % 4;
```

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
    Poom.audio().loadEnabled();
    Poom.audio().play(pickup, true); // true repeats the sequence
}

void loop()
{
    Poom.update();
}
```

Use `{0, duration}` for a rest and `{0, 0}` to end the sequence.
`setEnabled()` changes the runtime setting; call `saveEnabled()` after the user
changes it. POOM stores the value in the game NVS partition under namespace
`"poom"`, key `"audio"`, and does not rewrite flash when it is already current.

## Persistent Game Data

Each game should use its own Preferences namespace:

```cpp
Poom.storage().begin("my_game");
const uint32_t highScore = Poom.storage().readUInt32("high_score", 0);
Poom.storage().writeUInt32("high_score", highScore + 100);

struct PlayerRecord { uint32_t score; uint8_t level; };
PlayerRecord record = {};
Poom.storage().readBytes("record", &record, sizeof(record));
Poom.storage().writeBytes("record", &record, sizeof(record));
```

Storage writes are skipped when the value has not changed.

For progress containing multiple fields, prefer one versioned record. It is
stored as a single CRC-protected blob, so incompatible or damaged records are
rejected instead of partially loading:

```cpp
struct PlayerRecord {
  uint32_t score;
  uint8_t level;
  uint8_t reserved[3];
};

PlayerRecord record = {};
if (!Poom.storage().readRecord("progress", 1, &record, sizeof(record))) {
  record = {}; // New game or incompatible save.
}
Poom.storage().writeRecord("progress", 1, &record, sizeof(record));
```

The default game-save partition is `"nvs"` and can be overridden at build time
with `POOM_GAME_NVS_PARTITION`. Production POOM firmware must reserve that
partition for games and keep system settings and secrets in a separate named
partition such as `"poom_nvs"`.

## Returning to the POOM launcher

When a game runs from the OTA1 game slot, `Poom.begin()` automatically enables
an exit chord. Holding A, B, and Down together for two seconds restarts the game
and lets POOM's boot policy return to the launcher. The chord is disabled for a
directly flashed application and can be controlled explicitly:

```cpp
Poom.enableExitChord(false);

if (Poom.runningFromLauncher()) {
  Poom.returnToLauncher(); // For an explicit "Exit" menu item.
}
```

Levels, maps, sprites, and other immutable assets should remain `const` data in
the compiled application. Persistent storage is only for progress and settings.

## IMU

POOM includes an LSM6DS3TR-C on the display I2C bus. Initialize it after
`Poom.begin()` and poll it once per game frame:

```cpp
#include <PoomIMU.h>

PoomIMU imu;

if (!imu.beginAuto()) {
  // Sensor was not found at 0x6B or 0x6A.
}

if (imu.poll()) {
  float tiltX = imu.accelerationG(PoomIMUAxisX);
  float spinZ = imu.angularRateDps(PoomIMUAxisZ);
}
```

The default setup matches the firmware driver: 104 Hz, +/-2 g, and +/-2000 dps.

## ESP-NOW multiplayer

ESP-NOW is opt-in, so single-player games do not allocate a multiplayer object
or initialize Wi-Fi. Two devices using the same game ID and channel discover
each other automatically:

```cpp
#include <PoomMultiplayer.h>

PoomMultiplayer multiplayer;
multiplayer.begin("my_game", 1);

void loop() {
  if (!Poom.nextFrame()) return;
  multiplayer.update(Poom.buttonState());

  if (multiplayer.remotePressed(PoomButtonA)) {
    // Remote player is holding A.
  }
}
```

Custom packets up to `POOM_MULTIPLAYER_MAX_PAYLOAD` bytes can be exchanged with
`send()`, `available()`, and `read()`. The base transport is intentionally
unreliable and unencrypted, like normal ESP-NOW broadcast; games should include
sequence/state information when an action must be recoverable.

For one pair of POOMs, automatic same-game discovery is enough and no pairing
screen is required. A session-code UI should only be added if multiple pairs
may run the same multiplayer game near one another and must choose which two
devices belong together.

## Examples

- `HelloPoom`: minimal OLED hello world
- `LedsDemo`: cycle through LED colors
- `BuzzerDemo`: play simple tones
- `ButtonsDemo`: show the pressed button on screen
- `SpriteDemo`: draw an animated bitmap sprite with Poom's native format

### Arduboy game ports

These examples are direct POOM API ports of existing Arduboy games, not
original POOM titles. Original authorship and licensing remain with their
respective creators; the POOM-specific work is the platform adaptation. Each
game directory includes its upstream license notice.

- `GamesArduboy/MysticBalloon`: [Mystic Balloon](https://github.com/Team-ARG-Museum/ID-34-Mystic-Balloon)
  by Team A.R.G. (GAVENO, CastPixel, JO3RI, and Martian220), MIT licensed. The
  POOM adaptation preserves its original 60 Hz game timing with adaptive 60/45
  Hz display presentation.
- `GamesArduboy/CatacombsOfTheDamned`: [Catacombs of the Damned](https://github.com/jhhoward/Arduboy3D)
  by James Howard, MIT licensed. The POOM adaptation uses a 45 Hz interpolated
  camera around the original 30 Hz game simulation with adaptive OLED
  presentation.
- `GamesArduboy/CASTLEBOY_AB`: [CastleBoy](https://github.com/jlauener/CastleBoy)
  by ZappedCow and Increment, MIT licensed. The POOM adaptation restores the
  original 60 Hz Arduboy simulation with adaptive 60/45 Hz display presentation.
- `GamesArduboy/HOLLOW_AB`: [Hollow Seeker](https://github.com/obono/ArduboyWorks/tree/master/hollow)
  by OBONO, MIT licensed. The POOM adaptation preserves its original 60 Hz
  game timing with adaptive 60/45 Hz display presentation.
- `GamesArduboy/PrinceOfArabia`: [Prince of Arabia](https://github.com/Press-Play-On-Tape/PrinceOfArabia)
  by Press Play on Tape, BSD-3-Clause licensed. The POOM adaptation embeds the
  FX level and art pack in application flash. See the game README for the full
  development, graphics, music, and original-concept credits.

## Tests

Run the host graphics and audio regression suites with:

```sh
make -C tests test
```

## License

Copyright (C) 2026 Poom.

This library is licensed under the GNU General Public License v3.0 or later.
See `LICENSE`.
