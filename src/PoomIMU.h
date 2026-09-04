#ifndef POOM_IMU_H
#define POOM_IMU_H

/**
 * @file PoomIMU.h
 * @brief Lightweight LSM6DS3TR-C motion sensor support for POOM games.
 */

#include <Arduino.h>
#include <Wire.h>

#include <stdint.h>

#include "PoomBoardConfig.h"

/** @brief Axis indexes used by PoomIMUSample and conversion helpers. */
enum PoomIMUAxis : uint8_t
{
    PoomIMUAxisX = 0,
    PoomIMUAxisY = 1,
    PoomIMUAxisZ = 2
};

/** @brief One raw LSM6DS3TR-C sample. */
struct PoomIMUSample
{
    /** Accelerometer readings at the configured +/-2 g range. */
    int16_t accelerationRaw[3];

    /** Gyroscope readings at the configured +/-2000 dps range. */
    int16_t angularRateRaw[3];

    /** Raw temperature reading. */
    int16_t temperatureRaw;

    /** Arduino millisecond timestamp captured after the I2C read. */
    uint32_t timestampMs;
};

/**
 * @brief Small polling driver for POOM's LSM6DS3TR-C IMU.
 *
 * Call begin() after Poom.begin() to share the display's initialized I2C bus.
 * The default configuration matches the POOM firmware: 104 Hz, +/-2 g, and
 * +/-2000 degrees per second.
 */
class PoomIMU
{
public:
    /** Initialize a sensor at a known 7-bit address. */
    bool begin(
        TwoWire &bus = Wire,
        uint8_t address = POOM_IMU_DEFAULT_ADDRESS,
        bool initializeBus = false
    );

    /** Try the default address followed by the alternate address. */
    bool beginAuto(TwoWire &bus = Wire, bool initializeBus = false);

    /** Put accelerometer and gyroscope into power-down and close the driver. */
    void end();

    /** @return true after successful sensor detection and configuration. */
    bool ready() const;

    /** @return detected WHO_AM_I value, normally 0x6A. */
    uint8_t deviceId() const;

    /** @return active 7-bit I2C address. */
    uint8_t address() const;

    /** @return true when accelerometer or gyroscope data is ready. */
    bool available();

    /** Read a fresh sample into the internal sample buffer. */
    bool poll();

    /** @return most recently completed sample. */
    const PoomIMUSample &sample() const;

    /** Convert one raw accelerometer axis to milli-g. */
    float accelerationMg(PoomIMUAxis axis) const;

    /** Convert one raw accelerometer axis to g. */
    float accelerationG(PoomIMUAxis axis) const;

    /** Convert one raw gyroscope axis to degrees per second. */
    float angularRateDps(PoomIMUAxis axis) const;

    /** Convert the raw temperature sample to degrees Celsius. */
    float temperatureC() const;

private:
    TwoWire *wire_ = nullptr;
    uint8_t address_ = 0;
    uint8_t deviceId_ = 0;
    bool ready_ = false;
    PoomIMUSample sample_ = {};

    bool configure();
    bool writeRegister(uint8_t reg, uint8_t value);
    bool readRegister(uint8_t reg, uint8_t &value);
    bool readRegisters(uint8_t reg, uint8_t *values, size_t length);
};

#endif
