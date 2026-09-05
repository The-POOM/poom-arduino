# Third-Party Notices

POOM includes modified ports of third-party games and a few small compatibility
pieces needed by those ports. The notices below apply only to third-party code
or assets included in this repository. The POOM library itself is licensed
under GPL-3.0-or-later.

## Modified Game Ports

The games under `examples/GamesArduboy` are modified ports, not original POOM
titles.

- Mystic Balloon, Catacombs of the Damned, CastleBoy, and Hollow Seeker are
  distributed under their upstream MIT licenses.
- Prince of Arabia is distributed under its upstream BSD 3-Clause license.
- Each game directory retains its upstream `LICENSE` file.
- Additional authorship and project links are listed in the main README.

## Included Compatibility Code

Prince of Arabia includes modified ArduboyTones compatibility code. Its
original MIT notice is retained in
`examples/GamesArduboy/PrinceOfArabia/src/ArduboyTonesFX.h`.

Catacombs of the Damned includes a compact MIT-licensed font. The source note
is retained in `examples/GamesArduboy/CatacombsOfTheDamned/Font.cpp`.

POOM implements Arduboy-compatible compressed bitmap drawing in
`src/PoomGraphics.cpp` for game and sprite data that uses that format.

## Team A.R.G. Compressed Drawing - MIT

Code for drawing compressed bitmaps:
Copyright (c) 2016 TEAM a.r.g.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
