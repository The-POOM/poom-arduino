#include <assert.h>
#include <math.h>

#include "PoomIMU.h"

TwoWire Wire;
static unsigned long testMillis = 0;

unsigned long millis() { return testMillis; }
unsigned long micros() { return testMillis * 1000UL; }
void delay(unsigned long duration) { testMillis += duration; }

static void setSigned16(TwoWire &wire, uint8_t reg, int16_t value)
{
    wire.setRegister(reg, static_cast<uint8_t>(value & 0xFF));
    wire.setRegister(reg + 1, static_cast<uint8_t>((static_cast<uint16_t>(value) >> 8) & 0xFF));
}

int main()
{
    Wire.setDevice(POOM_IMU_DEFAULT_ADDRESS);
    Wire.setRegister(0x0F, 0x6A);

    PoomIMU imu;
    assert(imu.begin());
    assert(imu.ready());
    assert(imu.deviceId() == 0x6A);
    assert(imu.address() == POOM_IMU_DEFAULT_ADDRESS);
    assert(Wire.getRegister(0x10) == 0x40);
    assert(Wire.getRegister(0x11) == 0x4C);
    assert(Wire.getRegister(0x12) == 0x44);
    assert(Wire.clock() == POOM_IMU_I2C_CLOCK_HZ);

    Wire.setRegister(0x1E, 0x03);
    setSigned16(Wire, 0x20, 16);
    setSigned16(Wire, 0x22, 1000);
    setSigned16(Wire, 0x24, -1000);
    setSigned16(Wire, 0x26, 500);
    setSigned16(Wire, 0x28, 16384);
    setSigned16(Wire, 0x2A, -16384);
    setSigned16(Wire, 0x2C, 0);

    Wire.resetTransmissionLog();
    assert(imu.poll());
    assert(Wire.transactionCount() == 1);
    assert(imu.sample().angularRateRaw[PoomIMUAxisX] == 1000);
    assert(imu.sample().accelerationRaw[PoomIMUAxisY] == -16384);
    assert(fabs(imu.angularRateDps(PoomIMUAxisX) - 70.0f) < 0.01f);
    assert(fabs(imu.accelerationG(PoomIMUAxisX) - 0.999424f) < 0.001f);
    assert(fabs(imu.temperatureC() - 26.0f) < 0.01f);

    Wire.setRegister(0x1E, 0);
    assert(!imu.poll());
    imu.end();
    assert(!imu.ready());

    TwoWire missing;
    missing.setDevice(POOM_IMU_DEFAULT_ADDRESS);
    missing.setRegister(0x0F, 0xFF);
    assert(!imu.begin(missing));

    return 0;
}
