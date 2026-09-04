#include "PoomStorage.h"

#include <stdlib.h>
#include <string.h>

#if defined(ARDUINO_ARCH_ESP32)
namespace
{
constexpr uint32_t PoomRecordMagic = 0x4D4F4F50UL; // "POOM" in little endian.
constexpr uint16_t PoomRecordFormatVersion = 1;

struct PoomRecordHeader
{
    uint32_t magic;
    uint16_t formatVersion;
    uint16_t schemaVersion;
    uint32_t payloadLength;
    uint32_t payloadCrc32;
};

static_assert(sizeof(PoomRecordHeader) == 16, "Unexpected Poom record header layout");

uint32_t calculateCrc32(const void *value, size_t length)
{
    const uint8_t *bytes = static_cast<const uint8_t *>(value);
    uint32_t crc = 0xFFFFFFFFUL;

    for (size_t index = 0; index < length; ++index) {
        crc ^= bytes[index];
        for (uint8_t bit = 0; bit < 8; ++bit) {
            const uint32_t mask = 0U - (crc & 1U);
            crc = (crc >> 1U) ^ (0xEDB88320UL & mask);
        }
    }

    return ~crc;
}
}
#endif

bool PoomStorage::begin(
    const char *namespaceName,
    bool readOnly,
    const char *partitionLabel
)
{
    end();
#if defined(ARDUINO_ARCH_ESP32)
    open_ = namespaceName && partitionLabel &&
        preferences_.begin(namespaceName, readOnly, partitionLabel);
#else
    (void)namespaceName;
    (void)readOnly;
    (void)partitionLabel;
    open_ = false;
#endif
    return open_;
}

void PoomStorage::end()
{
#if defined(ARDUINO_ARCH_ESP32)
    if (open_) {
        preferences_.end();
    }
#endif
    open_ = false;
}

bool PoomStorage::contains(const char *key)
{
#if defined(ARDUINO_ARCH_ESP32)
    return open_ && key && preferences_.isKey(key);
#else
    (void)key;
    return false;
#endif
}

uint8_t PoomStorage::readByte(const char *key, uint8_t defaultValue)
{
#if defined(ARDUINO_ARCH_ESP32)
    return open_ && key ? preferences_.getUChar(key, defaultValue) : defaultValue;
#else
    (void)key;
    return defaultValue;
#endif
}

uint32_t PoomStorage::readUInt32(const char *key, uint32_t defaultValue)
{
#if defined(ARDUINO_ARCH_ESP32)
    return open_ && key ? preferences_.getULong(key, defaultValue) : defaultValue;
#else
    (void)key;
    return defaultValue;
#endif
}

bool PoomStorage::writeByte(const char *key, uint8_t value)
{
#if defined(ARDUINO_ARCH_ESP32)
    if (!open_ || !key) {
        return false;
    }
    if (preferences_.isKey(key) && preferences_.getUChar(key) == value) {
        return true;
    }
    return preferences_.putUChar(key, value) == sizeof(value);
#else
    (void)key;
    (void)value;
    return false;
#endif
}

bool PoomStorage::writeUInt32(const char *key, uint32_t value)
{
#if defined(ARDUINO_ARCH_ESP32)
    if (!open_ || !key) {
        return false;
    }
    if (preferences_.isKey(key) && preferences_.getULong(key) == value) {
        return true;
    }
    return preferences_.putULong(key, value) == sizeof(value);
#else
    (void)key;
    (void)value;
    return false;
#endif
}

size_t PoomStorage::readBytes(const char *key, void *value, size_t length)
{
#if defined(ARDUINO_ARCH_ESP32)
    if (!open_ || !key || !value || length == 0) {
        return 0;
    }
    return preferences_.getBytes(key, value, length);
#else
    (void)key;
    (void)value;
    (void)length;
    return 0;
#endif
}

bool PoomStorage::writeBytes(const char *key, const void *value, size_t length)
{
#if defined(ARDUINO_ARCH_ESP32)
    if (!open_ || !key || !value || length == 0) {
        return false;
    }

    if (preferences_.getBytesLength(key) == length) {
        void *stored = malloc(length);
        if (stored) {
            const bool unchanged = preferences_.getBytes(key, stored, length) == length &&
                memcmp(stored, value, length) == 0;
            free(stored);
            if (unchanged) {
                return true;
            }
        }
    }

    return preferences_.putBytes(key, value, length) == length;
#else
    (void)key;
    (void)value;
    (void)length;
    return false;
#endif
}

bool PoomStorage::readRecord(
    const char *key,
    uint16_t schemaVersion,
    void *value,
    size_t length
)
{
#if defined(ARDUINO_ARCH_ESP32)
    if (!open_ || !key || !value || length == 0 ||
        length > UINT32_MAX || length > SIZE_MAX - sizeof(PoomRecordHeader)) {
        return false;
    }

    const size_t storedLength = preferences_.getBytesLength(key);
    if (storedLength != sizeof(PoomRecordHeader) + length) {
        return false;
    }

    uint8_t *stored = static_cast<uint8_t *>(malloc(storedLength));
    if (!stored) {
        return false;
    }

    const bool read = preferences_.getBytes(key, stored, storedLength) == storedLength;
    PoomRecordHeader header = {};
    if (read) {
        memcpy(&header, stored, sizeof(header));
    }

    const uint8_t *payload = stored + sizeof(header);
    const bool valid = read &&
        header.magic == PoomRecordMagic &&
        header.formatVersion == PoomRecordFormatVersion &&
        header.schemaVersion == schemaVersion &&
        header.payloadLength == length &&
        header.payloadCrc32 == calculateCrc32(payload, length);

    if (valid) {
        memcpy(value, payload, length);
    }
    free(stored);
    return valid;
#else
    (void)key;
    (void)schemaVersion;
    (void)value;
    (void)length;
    return false;
#endif
}

bool PoomStorage::writeRecord(
    const char *key,
    uint16_t schemaVersion,
    const void *value,
    size_t length
)
{
#if defined(ARDUINO_ARCH_ESP32)
    if (!open_ || !key || !value || length == 0 ||
        length > UINT32_MAX || length > SIZE_MAX - sizeof(PoomRecordHeader)) {
        return false;
    }

    const size_t storedLength = sizeof(PoomRecordHeader) + length;
    uint8_t *stored = static_cast<uint8_t *>(malloc(storedLength));
    if (!stored) {
        return false;
    }

    const PoomRecordHeader header = {
        PoomRecordMagic,
        PoomRecordFormatVersion,
        schemaVersion,
        static_cast<uint32_t>(length),
        calculateCrc32(value, length)
    };
    memcpy(stored, &header, sizeof(header));
    memcpy(stored + sizeof(header), value, length);

    const bool written = writeBytes(key, stored, storedLength);
    free(stored);
    return written;
#else
    (void)key;
    (void)schemaVersion;
    (void)value;
    (void)length;
    return false;
#endif
}
