#ifndef TEST_WIRE_H
#define TEST_WIRE_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define WIRE_HAS_BUFFER_SIZE 1

class TwoWire
{
public:
    static constexpr size_t MaximumTransactions = 128;
    static constexpr size_t MaximumTransmissionLength = 130;

    bool begin(int sda, int scl)
    {
        (void)sda;
        (void)scl;
        return true;
    }

    void setDevice(uint8_t address, bool present = true)
    {
        deviceAddress_ = address;
        present_ = present;
    }

    void setRegister(uint8_t reg, uint8_t value)
    {
        registers_[reg] = value;
    }

    uint8_t getRegister(uint8_t reg) const
    {
        return registers_[reg];
    }

    void beginTransmission(uint8_t address)
    {
        transmissionAddress_ = address;
        transmitLength_ = 0;
    }

    size_t write(uint8_t value)
    {
        if (transmitLength_ < bufferSize_ && transmitLength_ < sizeof(transmit_)) {
            transmit_[transmitLength_++] = value;
            return 1;
        }
        return 0;
    }

    size_t write(const uint8_t *data, size_t length)
    {
        size_t written = 0;
        while (written < length && write(data[written]) == 1) {
            ++written;
        }
        return written;
    }

    void setClock(uint32_t clockHz)
    {
        clockHz_ = clockHz;
    }

    size_t setBufferSize(size_t size)
    {
        bufferSize_ = size < MaximumTransmissionLength
            ? size
            : MaximumTransmissionLength;
        return bufferSize_;
    }

    uint32_t clock() const
    {
        return clockHz_;
    }

    void resetTransmissionLog()
    {
        transactionCount_ = 0;
    }

    size_t transactionCount() const
    {
        return transactionCount_;
    }

    size_t transactionLength(size_t index) const
    {
        return index < transactionCount_ ? transactionLengths_[index] : 0;
    }

    uint8_t transactionByte(size_t transaction, size_t index) const
    {
        return transaction < transactionCount_ && index < transactionLengths_[transaction]
            ? transactions_[transaction][index]
            : 0;
    }

    uint8_t endTransmission(bool stop = true)
    {
        (void)stop;
        if (!present_ || transmissionAddress_ != deviceAddress_ || transmitLength_ == 0) {
            return 1;
        }

        if (transactionCount_ < MaximumTransactions) {
            transactionLengths_[transactionCount_] = transmitLength_;
            memcpy(
                transactions_[transactionCount_],
                transmit_,
                transmitLength_
            );
            ++transactionCount_;
        }

        currentRegister_ = transmit_[0];
        for (size_t index = 1; index < transmitLength_; ++index) {
            registers_[currentRegister_++] = transmit_[index];
        }

        // Simulate the LSM6DS3TR-C clearing SW_RESET after reset completes.
        if (transmitLength_ == 2 && transmit_[0] == 0x12 && (transmit_[1] & 0x01)) {
            registers_[0x12] = 0;
        }
        return 0;
    }

    uint8_t requestFrom(uint8_t address, uint8_t length)
    {
        if (!present_ || address != deviceAddress_) {
            receiveLength_ = 0;
            return 0;
        }
        receiveLength_ = length;
        receiveIndex_ = 0;
        for (uint8_t index = 0; index < length; ++index) {
            receive_[index] = registers_[static_cast<uint8_t>(currentRegister_ + index)];
        }
        currentRegister_ = static_cast<uint8_t>(currentRegister_ + length);
        return length;
    }

    int read()
    {
        return receiveIndex_ < receiveLength_ ? receive_[receiveIndex_++] : -1;
    }

private:
    bool present_ = false;
    uint8_t deviceAddress_ = 0;
    uint8_t transmissionAddress_ = 0;
    uint8_t currentRegister_ = 0;
    uint32_t clockHz_ = 0;
    size_t bufferSize_ = 128;
    uint8_t registers_[256] = {0};
    uint8_t transmit_[MaximumTransmissionLength] = {0};
    size_t transmitLength_ = 0;
    uint8_t transactions_[MaximumTransactions][MaximumTransmissionLength] = {{0}};
    size_t transactionLengths_[MaximumTransactions] = {0};
    size_t transactionCount_ = 0;
    uint8_t receive_[255] = {0};
    uint8_t receiveLength_ = 0;
    uint8_t receiveIndex_ = 0;
};

extern TwoWire Wire;

#endif
