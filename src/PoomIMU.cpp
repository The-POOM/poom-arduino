#include "PoomIMU.h"

namespace
{
constexpr uint8_t RegisterWhoAmI = 0x0F;
constexpr uint8_t RegisterControlAccel = 0x10;
constexpr uint8_t RegisterControlGyro = 0x11;
constexpr uint8_t RegisterControlCommon = 0x12;
constexpr uint8_t RegisterStatus = 0x1E;

constexpr uint8_t ExpectedDeviceId = 0x6A;
constexpr uint8_t StatusAccelReady = 0x01;
constexpr uint8_t StatusGyroReady = 0x02;
constexpr uint8_t SoftwareReset = 0x01;
constexpr uint32_t ResetTimeoutMs = 100;

constexpr float AccelerationMgPerLsb = 0.061f;
constexpr float AngularRateDpsPerLsb = 0.070f;

int16_t readSigned16(const uint8_t *bytes)
{
    return static_cast<int16_t>(
        static_cast<uint16_t>(bytes[0]) |
        (static_cast<uint16_t>(bytes[1]) << 8U)
    );
}
}

bool PoomIMU::begin(TwoWire &bus, uint8_t address, bool initializeBus)
{
    end();
    wire_ = &bus;
    address_ = address & 0x7FU;

    if (initializeBus) {
        wire_->begin(POOM_I2C_SDA_PIN, POOM_I2C_SCL_PIN);
    }
    wire_->setClock(POOM_IMU_I2C_CLOCK_HZ);

    if (!readRegister(RegisterWhoAmI, deviceId_) || deviceId_ != ExpectedDeviceId) {
        return false;
    }

    if (!configure()) {
        return false;
    }

    ready_ = true;
    return true;
}

bool PoomIMU::beginAuto(TwoWire &bus, bool initializeBus)
{
    if (begin(bus, POOM_IMU_DEFAULT_ADDRESS, initializeBus)) {
        return true;
    }
    return begin(bus, POOM_IMU_ALTERNATE_ADDRESS, false);
}

void PoomIMU::end()
{
    if (ready_ && wire_) {
        (void)writeRegister(RegisterControlAccel, 0);
        (void)writeRegister(RegisterControlGyro, 0);
    }
    ready_ = false;
    deviceId_ = 0;
}

bool PoomIMU::ready() const
{
    return ready_;
}

uint8_t PoomIMU::deviceId() const
{
    return deviceId_;
}

uint8_t PoomIMU::address() const
{
    return address_;
}

bool PoomIMU::available()
{
    if (!ready_) {
        return false;
    }

    uint8_t status = 0;
    return readRegister(RegisterStatus, status) &&
        (status & (StatusAccelReady | StatusGyroReady)) != 0;
}

bool PoomIMU::poll()
{
    // Status, temperature, gyro, and accelerometer fit in one contiguous read.
    // Starting at STATUS_REG removes the extra I2C transaction that a separate
    // available() check would require.
    uint8_t values[16] = {0};
    if (!ready_ || !readRegisters(RegisterStatus, values, sizeof(values)) ||
        (values[0] & (StatusAccelReady | StatusGyroReady)) == 0) {
        return false;
    }

    const uint8_t *sensorData = values + 2;
    sample_.temperatureRaw = readSigned16(sensorData);
    for (uint8_t axis = 0; axis < 3; ++axis) {
        sample_.angularRateRaw[axis] = readSigned16(sensorData + 2U + axis * 2U);
        sample_.accelerationRaw[axis] = readSigned16(sensorData + 8U + axis * 2U);
    }
    sample_.timestampMs = millis();
    return true;
}

const PoomIMUSample &PoomIMU::sample() const
{
    return sample_;
}

float PoomIMU::accelerationMg(PoomIMUAxis axis) const
{
    return sample_.accelerationRaw[static_cast<uint8_t>(axis)] * AccelerationMgPerLsb;
}

float PoomIMU::accelerationG(PoomIMUAxis axis) const
{
    return accelerationMg(axis) / 1000.0f;
}

float PoomIMU::angularRateDps(PoomIMUAxis axis) const
{
    return sample_.angularRateRaw[static_cast<uint8_t>(axis)] * AngularRateDpsPerLsb;
}

float PoomIMU::temperatureC() const
{
    return 25.0f + static_cast<float>(sample_.temperatureRaw) / 16.0f;
}

bool PoomIMU::configure()
{
    if (!writeRegister(RegisterControlCommon, SoftwareReset)) {
        return false;
    }

    const uint32_t startedAt = millis();
    uint8_t control = SoftwareReset;
    do {
        delay(1);
        if (!readRegister(RegisterControlCommon, control)) {
            return false;
        }
    } while ((control & SoftwareReset) != 0 &&
             static_cast<uint32_t>(millis() - startedAt) < ResetTimeoutMs);

    if ((control & SoftwareReset) != 0) {
        return false;
    }

    // BDU=1 and IF_INC=1.
    if (!writeRegister(RegisterControlCommon, 0x44)) {
        return false;
    }
    // Accelerometer: 104 Hz, +/-2 g.
    if (!writeRegister(RegisterControlAccel, 0x40)) {
        return false;
    }
    // Gyroscope: 104 Hz, +/-2000 dps.
    return writeRegister(RegisterControlGyro, 0x4C);
}

bool PoomIMU::writeRegister(uint8_t reg, uint8_t value)
{
    if (!wire_) {
        return false;
    }
    wire_->setClock(POOM_IMU_I2C_CLOCK_HZ);
    wire_->beginTransmission(address_);
    wire_->write(reg);
    wire_->write(value);
    return wire_->endTransmission() == 0;
}

bool PoomIMU::readRegister(uint8_t reg, uint8_t &value)
{
    return readRegisters(reg, &value, 1);
}

bool PoomIMU::readRegisters(uint8_t reg, uint8_t *values, size_t length)
{
    if (!wire_ || !values || length == 0 || length > 255) {
        return false;
    }

    wire_->setClock(POOM_IMU_I2C_CLOCK_HZ);
    wire_->beginTransmission(address_);
    wire_->write(reg);
    if (wire_->endTransmission(false) != 0) {
        return false;
    }

    const uint8_t requested = static_cast<uint8_t>(length);
    if (wire_->requestFrom(address_, requested) != requested) {
        return false;
    }

    for (size_t index = 0; index < length; ++index) {
        values[index] = static_cast<uint8_t>(wire_->read());
    }
    return true;
}
