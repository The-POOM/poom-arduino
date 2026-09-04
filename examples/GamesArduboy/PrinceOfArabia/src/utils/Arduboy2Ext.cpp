#include "Arduboy2Ext.h"

Arduboy2Ext::Arduboy2Ext() : sBuffer(Poom.buffer()) { }

void PoomAudioCompat::begin() {
  Poom.audio().loadEnabled();
}

bool PoomAudioCompat::enabled() {
  return Poom.audio().enabled();
}

void PoomAudioCompat::toggle() {
  Poom.audio().setEnabled(!Poom.audio().enabled());
}

void PoomAudioCompat::on() {
  Poom.audio().setEnabled(true);
}

void PoomAudioCompat::off() {
  Poom.audio().setEnabled(false);
}

void Arduboy2Ext::boot() {
  Poom.begin();
  Poom.storage().begin("prince_arabia");
  sBuffer = Poom.buffer();
}

void Arduboy2Ext::systemButtons() { }

bool Arduboy2Ext::nextFrame() {
  if (!Poom.nextFrame()) return false;
  ++frameCount;
  return true;
}

void Arduboy2Ext::pollButtons() {
  previousButtonState = currentButtonState;
  currentButtonState = Poom.buttonState();
}

void Arduboy2Ext::setFrameRate(uint8_t rate) {
  Poom.setFrameRate(rate);
}

void Arduboy2Ext::invert(bool inverted) {
  if (inverted) Poom.invert();
}

void Arduboy2Ext::fillRect(
  int16_t x,
  int16_t y,
  uint8_t width,
  uint8_t height,
  uint8_t color
) {
  Poom.fillRect(x, y, width, height, color != BLACK);
}

void Arduboy2Ext::setRGBled(uint8_t red, uint8_t green, uint8_t blue) {
  ledRed_ = red;
  ledGreen_ = green;
  ledBlue_ = blue;
  showLed();
}

void Arduboy2Ext::setRGBled(uint8_t color, uint8_t value) {
  if (color == RED_LED) ledRed_ = value;
  else if (color == GREEN_LED) ledGreen_ = value;
  else if (color == BLUE_LED) ledBlue_ = value;
  showLed();
}

void Arduboy2Ext::setRGBledRedOn() { ledRed_ = 255; showLed(); }
void Arduboy2Ext::setRGBledBlueOn() { ledBlue_ = 255; showLed(); }
void Arduboy2Ext::setRGBledRedOff() { ledRed_ = 0; showLed(); }
void Arduboy2Ext::setRGBledGreenOff() { ledGreen_ = 0; showLed(); }
void Arduboy2Ext::setRGBledBlueOff() { ledBlue_ = 0; showLed(); }
void Arduboy2Ext::setRGBledGreenOn() { ledGreen_ = 255; showLed(); }

void Arduboy2Ext::showLed() {
  Poom.leds().setColor(ledRed_, ledGreen_, ledBlue_);
}

bool Arduboy2Ext::collide(const Point &point, const Rect &rect) {
  return point.x >= rect.x && point.x < rect.x + rect.width &&
    point.y >= rect.y && point.y < rect.y + rect.height;
}

bool Arduboy2Ext::collide(const Rect &first, const Rect &second) {
  return first.x < second.x + second.width && first.x + first.width > second.x &&
    first.y < second.y + second.height && first.y + first.height > second.y;
}

uint8_t Arduboy2Ext::justPressedButtons() const {

  return (~previousButtonState & currentButtonState);

}

uint8_t Arduboy2Ext::pressedButtons() const {

  return currentButtonState;

}

void Arduboy2Ext::clearButtonState() {

  currentButtonState = previousButtonState = 0;

}


void Arduboy2Ext::resetFrameCount() {

  frameCount = 0;

}

uint16_t Arduboy2Ext::getFrameCount() const {

  return frameCount;

}

void Arduboy2Ext::setFrameCount(uint16_t val) const {

  frameCount = val;

}

uint8_t Arduboy2Ext::getFrameCount(uint8_t mod, int8_t offset) const {

  return (frameCount + offset) % mod;

}

bool Arduboy2Ext::getFrameCountHalf(uint8_t mod) const {

	return getFrameCount(mod) > (mod / 2);

}

bool Arduboy2Ext::isFrameCount(uint8_t mod) const {

  return (frameCount % mod) == 0;

}

bool Arduboy2Ext::isFrameCount(uint8_t mod, uint8_t val) const {

  return (frameCount % mod) == val;

}


uint16_t rnd = 0xACE1;


uint8_t Arduboy2Ext::randomLFSR(uint8_t min, uint8_t max) {
  uint16_t r = rnd;
  r ^= static_cast<uint16_t>(micros());
  (r & 1) ? r = (r >> 1) ^ 0xB400 : r >>= 1;
  rnd = r;
  if (max <= min) return min;
  return r % (max - min) + min;
}
