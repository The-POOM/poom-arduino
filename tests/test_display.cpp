#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "PoomDisplay.h"

TwoWire Wire;
static unsigned long testMicros = 0;

void pinMode(uint8_t, uint8_t) { }
unsigned long millis() { return testMicros / 1000UL; }
unsigned long micros() { testMicros += 100UL; return testMicros; }
void delay(unsigned long duration) { testMicros += duration * 1000UL; }
void tone(uint8_t, unsigned int) { }
void tone(uint8_t, unsigned int, unsigned long) { }
void noTone(uint8_t) { }

int main()
{
    Wire.setDevice(POOM_DISPLAY_I2C_ADDR);
    PoomDisplay display;
    assert(display.begin());
    assert(display.i2cClock() == POOM_DISPLAY_I2C_CLOCK_HZ);

    uint8_t framebuffer[PoomFramebufferSize] = {};
    Wire.resetTransmissionLog();
    display.drawBuffer(framebuffer);
    assert(display.metrics().presentCount == 1);
    assert(display.metrics().fullTransferCount == 1);
    assert(display.metrics().lastDataBytes == PoomFramebufferSize);
    assert(Wire.transactionCount() == 16);

    Wire.resetTransmissionLog();
    display.drawBuffer(framebuffer);
    assert(display.metrics().presentCount == 2);
    assert(display.metrics().skippedCount == 1);
    assert(display.metrics().lastDataBytes == 0);
    assert(Wire.transactionCount() == 0);

    framebuffer[2 * PoomScreenWidth + 10] = 0xA5;
    Wire.resetTransmissionLog();
    display.drawBuffer(framebuffer);
    assert(display.metrics().partialTransferCount == 1);
    assert(display.metrics().lastDataBytes == 1);
    assert(Wire.transactionCount() == 2);
    assert(Wire.transactionLength(0) == 4);
    assert(Wire.transactionByte(0, 0) == 0x00);
    assert(Wire.transactionByte(0, 1) == 0x0C);
    assert(Wire.transactionByte(0, 2) == 0x10);
    assert(Wire.transactionByte(0, 3) == 0xB2);
    assert(Wire.transactionLength(1) == 2);
    assert(Wire.transactionByte(1, 0) == 0x40);
    assert(Wire.transactionByte(1, 1) == 0xA5);

    framebuffer[20] = 0x11;
    framebuffer[22] = 0x22;
    Wire.resetTransmissionLog();
    display.drawBuffer(framebuffer);
    assert(display.metrics().lastDataBytes == 3);
    assert(Wire.transactionCount() == 2);
    assert(Wire.transactionLength(1) == 4);
    assert(Wire.transactionByte(1, 1) == 0x11);
    assert(Wire.transactionByte(1, 2) == 0x00);
    assert(Wire.transactionByte(1, 3) == 0x22);

    display.setI2CClock(800000UL);
    framebuffer[23] = 0x33;
    display.drawBuffer(framebuffer);
    assert(display.i2cClock() == 800000UL);
    assert(Wire.clock() == 800000UL);

    const uint32_t presentationsBeforeNativeShow = display.metrics().presentCount;
    display.show();
    assert(display.metrics().presentCount == presentationsBeforeNativeShow + 1U);
    assert(display.metrics().lastDataBytes == 0);

    // drawBuffer keeps the native buffer synchronized, so either API can be
    // used without forcing a redundant full-screen transfer.
    Wire.resetTransmissionLog();
    display.drawBuffer(framebuffer);
    assert(display.metrics().lastDataBytes == 0);
    assert(Wire.transactionCount() == 0);

    puts("All display tests passed");
    return 0;
}
