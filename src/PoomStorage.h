#ifndef POOM_STORAGE_H
#define POOM_STORAGE_H

/**
 * @file PoomStorage.h
 * @brief Namespaced persistent storage for Poom applications.
 */

#include <stddef.h>
#include <stdint.h>

#if defined(ARDUINO_ARCH_ESP32)
#include <Preferences.h>
#endif

/**
 * @def POOM_GAME_NVS_PARTITION
 * @brief NVS partition reserved for game and shared POOM-library settings.
 *
 * Arduino initializes the partition named "nvs" during startup, so POOM games
 * use that partition by default. Production POOM firmware must keep its system
 * settings in a differently named partition, such as "poom_nvs".
 */
#ifndef POOM_GAME_NVS_PARTITION
#define POOM_GAME_NVS_PARTITION "nvs"
#endif

/**
 * @brief Small ESP32 Preferences wrapper that avoids unchanged flash writes.
 *
 * Use a distinct namespace for each game or application. Keys and namespaces
 * follow the length limits imposed by ESP32 Preferences.
 */
class PoomStorage
{
public:
    /**
     * @brief Open a Preferences namespace in the game-save partition.
     * @param namespaceName Stable game identifier, limited to 15 characters.
     * @param readOnly true to prevent writes through this handle.
     * @param partitionLabel NVS partition reserved for game data.
     */
    bool begin(
        const char *namespaceName,
        bool readOnly = false,
        const char *partitionLabel = POOM_GAME_NVS_PARTITION
    );

    /** @brief Close the current Preferences namespace. */
    void end();

    /** @brief Test whether the open namespace contains a key. */
    bool contains(const char *key);

    /** @brief Read an unsigned byte, returning defaultValue when absent. */
    uint8_t readByte(const char *key, uint8_t defaultValue = 0);

    /** @brief Read an unsigned 32-bit value, returning defaultValue when absent. */
    uint32_t readUInt32(const char *key, uint32_t defaultValue = 0);

    /** @brief Write an unsigned byte only when its stored value differs. */
    bool writeByte(const char *key, uint8_t value);

    /** @brief Write an unsigned 32-bit value only when its stored value differs. */
    bool writeUInt32(const char *key, uint32_t value);

    /** @brief Read up to length bytes from a stored blob. */
    size_t readBytes(const char *key, void *value, size_t length);

    /** @brief Write a blob only when its stored bytes differ. */
    bool writeBytes(const char *key, const void *value, size_t length);

    /**
     * @brief Read and validate one versioned record.
     * @return true only when the record header, version, length, and CRC match.
     */
    bool readRecord(
        const char *key,
        uint16_t schemaVersion,
        void *value,
        size_t length
    );

    /**
     * @brief Store one versioned, CRC-protected record as a single NVS blob.
     * @return true when stored successfully or already unchanged.
     */
    bool writeRecord(
        const char *key,
        uint16_t schemaVersion,
        const void *value,
        size_t length
    );

private:
    bool open_ = false;

#if defined(ARDUINO_ARCH_ESP32)
    Preferences preferences_;
#endif
};

#endif
