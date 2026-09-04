#ifndef POOM_MULTIPLAYER_H
#define POOM_MULTIPLAYER_H

/**
 * @file PoomMultiplayer.h
 * @brief Small two-player ESP-NOW link for POOM games.
 */

#include <Arduino.h>

#include <stddef.h>
#include <stdint.h>

#if defined(ARDUINO_ARCH_ESP32)
#include <esp_now.h>
#endif

#ifndef POOM_MULTIPLAYER_DEFAULT_CHANNEL
#define POOM_MULTIPLAYER_DEFAULT_CHANNEL 1
#endif

#ifndef POOM_MULTIPLAYER_SEND_INTERVAL_MS
#define POOM_MULTIPLAYER_SEND_INTERVAL_MS 33
#endif

#ifndef POOM_MULTIPLAYER_STALE_MS
#define POOM_MULTIPLAYER_STALE_MS 1000
#endif

#ifndef POOM_MULTIPLAYER_MAX_PAYLOAD
#define POOM_MULTIPLAYER_MAX_PAYLOAD 64
#endif

/**
 * @brief Opt-in ESP-NOW transport for two copies of the same game.
 *
 * Peers discover each other through a game-specific broadcast hash. Only one
 * PoomMultiplayer instance can own ESP-NOW callbacks at a time.
 */
class PoomMultiplayer
{
public:
    enum PacketType : uint8_t
    {
        PacketTypeHello = 1,
        PacketTypeButtons = 2,
        PacketTypeUser = 16
    };

    PoomMultiplayer();

    /** Start discovery for one game on a fixed Wi-Fi channel. */
    bool begin(
        const char *gameId,
        uint8_t channel = POOM_MULTIPLAYER_DEFAULT_CHANNEL,
        uint8_t localPlayer = 0
    );

    /** Stop ESP-NOW and release the radio callbacks owned by this instance. */
    void end();

    bool enabled() const;
    bool connected(uint32_t staleMs = POOM_MULTIPLAYER_STALE_MS) const;

    /** Send local buttons when changed and at the configured keepalive rate. */
    void update(uint8_t localButtons);

    uint8_t remoteButtons() const;
    uint8_t remoteJustPressed() const;
    uint8_t remoteJustReleased() const;
    bool remotePressed(uint8_t buttons) const;

    /** Queue a game-specific payload for broadcast. */
    bool send(
        const void *payload,
        uint8_t length,
        uint8_t type = PacketTypeUser
    );

    /** @return true when one unread user packet is waiting. */
    bool available() const;

    /** Copy and consume the most recently received user packet. */
    uint8_t read(
        void *buffer,
        uint8_t maxLength,
        uint8_t *type = nullptr,
        uint32_t *from = nullptr
    );

    /** Forget the current peer and unread packet. */
    void clear();

    uint32_t localId() const;
    uint32_t peerId() const;
    uint8_t localPlayer() const;
    uint8_t peerPlayer() const;
    uint8_t channel() const;
    uint32_t lastSeen() const;
    bool lastSendSucceeded() const;

private:
    bool enabled_;
    uint8_t channel_;
    uint8_t localPlayer_;
    uint8_t peerPlayer_;
    uint32_t gameHash_;
    uint32_t localId_;
    uint32_t peerId_;
    uint32_t sequence_;
    uint32_t lastReceivedSequence_;
    uint32_t lastSendMs_;
    uint32_t lastSeenMs_;
    uint8_t lastLocalButtons_;
    uint8_t remoteButtons_;
    uint8_t remotePreviousButtons_;
    uint8_t remoteJustPressed_;
    uint8_t remoteJustReleased_;
    uint8_t receivedButtons_;
    bool hasPeer_;
    bool hasPayload_;
    bool lastSendSucceeded_;
    uint8_t payloadType_;
    uint8_t payloadLength_;
    uint32_t payloadFrom_;
    uint8_t payload_[POOM_MULTIPLAYER_MAX_PAYLOAD];

    bool sendPacket(uint8_t type, const void *payload, uint8_t length, uint8_t buttons);
    void receivePacket(const uint8_t *data, int length);
    bool addBroadcastPeer();
    static uint32_t hashGameId(const char *gameId);
    static uint32_t makeLocalId();
    static bool sequenceIsNewer(uint32_t incoming, uint32_t previous);

#if defined(ARDUINO_ARCH_ESP32)
    static void onReceive(
        const esp_now_recv_info_t *info,
        const uint8_t *data,
        int length
    );
    static void onSent(
        const esp_now_send_info_t *info,
        esp_now_send_status_t status
    );
#endif
};

#include "PoomMultiplayer.inl"

#endif
