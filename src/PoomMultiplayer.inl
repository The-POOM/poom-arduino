#include <string.h>

#if defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#include <esp_mac.h>
#include <esp_wifi.h>
#include <freertos/FreeRTOS.h>
#endif

namespace PoomMultiplayerDetail
{
constexpr uint32_t PoomMultiplayerMagic = 0x504D5031UL; // "PMP1"
constexpr uint8_t PoomMultiplayerVersion = 1;

struct PoomMultiplayerPacket
{
    uint32_t magic;
    uint8_t version;
    uint8_t type;
    uint8_t player;
    uint8_t buttons;
    uint32_t gameHash;
    uint32_t senderId;
    uint32_t sequence;
    uint8_t payloadLength;
    uint8_t payload[POOM_MULTIPLAYER_MAX_PAYLOAD];
} __attribute__((packed));

constexpr uint8_t BroadcastAddress[6] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

inline PoomMultiplayer *&activeMultiplayer()
{
    static PoomMultiplayer *instance = nullptr;
    return instance;
}

#if defined(ARDUINO_ARCH_ESP32)
inline portMUX_TYPE &multiplayerMux()
{
    static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
    return mux;
}
#endif
}

inline PoomMultiplayer::PoomMultiplayer()
    : enabled_(false),
      channel_(POOM_MULTIPLAYER_DEFAULT_CHANNEL),
      localPlayer_(0),
      peerPlayer_(0),
      gameHash_(0),
      localId_(0),
      peerId_(0),
      sequence_(0),
      lastReceivedSequence_(0),
      lastSendMs_(0),
      lastSeenMs_(0),
      lastLocalButtons_(0),
      remoteButtons_(0),
      remotePreviousButtons_(0),
      remoteJustPressed_(0),
      remoteJustReleased_(0),
      receivedButtons_(0),
      hasPeer_(false),
      hasPayload_(false),
      lastSendSucceeded_(false),
      payloadType_(0),
      payloadLength_(0),
      payloadFrom_(0),
      payload_{0}
{
}

inline bool PoomMultiplayer::begin(const char *gameId, uint8_t channel, uint8_t localPlayer)
{
#if defined(ARDUINO_ARCH_ESP32)
    if (!gameId || gameId[0] == '\0' || channel < 1 || channel > 14 ||
        (PoomMultiplayerDetail::activeMultiplayer() &&
         PoomMultiplayerDetail::activeMultiplayer() != this)) {
        return false;
    }

    end();
    gameHash_ = hashGameId(gameId);
    channel_ = channel;
    localPlayer_ = localPlayer;
    localId_ = makeLocalId();
    sequence_ = 0;
    lastSendMs_ = 0;
    lastLocalButtons_ = 0;
    clear();

    WiFi.mode(WIFI_STA);
    WiFi.disconnect(false, false);
    if (esp_wifi_set_channel(channel_, WIFI_SECOND_CHAN_NONE) != ESP_OK) {
        return false;
    }

    if (esp_now_init() != ESP_OK) {
        return false;
    }

    PoomMultiplayerDetail::activeMultiplayer() = this;
    if (esp_now_register_recv_cb(PoomMultiplayer::onReceive) != ESP_OK ||
        esp_now_register_send_cb(PoomMultiplayer::onSent) != ESP_OK ||
        !addBroadcastPeer()) {
        esp_now_unregister_recv_cb();
        esp_now_unregister_send_cb();
        esp_now_deinit();
        PoomMultiplayerDetail::activeMultiplayer() = nullptr;
        return false;
    }

    enabled_ = true;
    (void)sendPacket(PacketTypeHello, nullptr, 0, 0);
    return true;
#else
    (void)gameId;
    (void)channel;
    (void)localPlayer;
    return false;
#endif
}

inline void PoomMultiplayer::end()
{
#if defined(ARDUINO_ARCH_ESP32)
    if (PoomMultiplayerDetail::activeMultiplayer() == this) {
        esp_now_unregister_recv_cb();
        esp_now_unregister_send_cb();
        esp_now_deinit();
        PoomMultiplayerDetail::activeMultiplayer() = nullptr;
    }
#endif
    enabled_ = false;
    clear();
}

inline bool PoomMultiplayer::enabled() const
{
    return enabled_;
}

inline bool PoomMultiplayer::connected(uint32_t staleMs) const
{
    bool hasPeer = false;
    uint32_t seenAt = 0;
#if defined(ARDUINO_ARCH_ESP32)
    portENTER_CRITICAL(&PoomMultiplayerDetail::multiplayerMux());
#endif
    hasPeer = hasPeer_;
    seenAt = lastSeenMs_;
#if defined(ARDUINO_ARCH_ESP32)
    portEXIT_CRITICAL(&PoomMultiplayerDetail::multiplayerMux());
#endif
    return hasPeer && static_cast<uint32_t>(millis() - seenAt) <= staleMs;
}

inline void PoomMultiplayer::update(uint8_t localButtons)
{
    bool hasPeer = false;
    uint8_t receivedButtons = 0;
    uint32_t seenAt = 0;
#if defined(ARDUINO_ARCH_ESP32)
    portENTER_CRITICAL(&PoomMultiplayerDetail::multiplayerMux());
#endif
    hasPeer = hasPeer_;
    receivedButtons = receivedButtons_;
    seenAt = lastSeenMs_;
#if defined(ARDUINO_ARCH_ESP32)
    portEXIT_CRITICAL(&PoomMultiplayerDetail::multiplayerMux());
#endif

    const uint32_t now = millis();
    if (!hasPeer || static_cast<uint32_t>(now - seenAt) > POOM_MULTIPLAYER_STALE_MS) {
        receivedButtons = 0;
    }

    remoteButtons_ = receivedButtons;
    remoteJustPressed_ = remoteButtons_ & static_cast<uint8_t>(~remotePreviousButtons_);
    remoteJustReleased_ = remotePreviousButtons_ & static_cast<uint8_t>(~remoteButtons_);
    remotePreviousButtons_ = remoteButtons_;

    if (!enabled_) {
        return;
    }

    if (localButtons != lastLocalButtons_ ||
        static_cast<uint32_t>(now - lastSendMs_) >= POOM_MULTIPLAYER_SEND_INTERVAL_MS) {
        (void)sendPacket(PacketTypeButtons, nullptr, 0, localButtons);
        lastLocalButtons_ = localButtons;
        lastSendMs_ = now;
    }
}

inline uint8_t PoomMultiplayer::remoteButtons() const
{
    return remoteButtons_;
}

inline uint8_t PoomMultiplayer::remoteJustPressed() const
{
    return remoteJustPressed_;
}

inline uint8_t PoomMultiplayer::remoteJustReleased() const
{
    return remoteJustReleased_;
}

inline bool PoomMultiplayer::remotePressed(uint8_t buttons) const
{
    return (remoteButtons_ & buttons) == buttons;
}

inline bool PoomMultiplayer::send(const void *payload, uint8_t length, uint8_t type)
{
    return sendPacket(type, payload, length, lastLocalButtons_);
}

inline bool PoomMultiplayer::available() const
{
    bool result = false;
#if defined(ARDUINO_ARCH_ESP32)
    portENTER_CRITICAL(&PoomMultiplayerDetail::multiplayerMux());
#endif
    result = hasPayload_;
#if defined(ARDUINO_ARCH_ESP32)
    portEXIT_CRITICAL(&PoomMultiplayerDetail::multiplayerMux());
#endif
    return result;
}

inline uint8_t PoomMultiplayer::read(
    void *buffer,
    uint8_t maxLength,
    uint8_t *type,
    uint32_t *from
)
{
    if (!buffer || maxLength == 0) {
        return 0;
    }

    uint8_t length = 0;
#if defined(ARDUINO_ARCH_ESP32)
    portENTER_CRITICAL(&PoomMultiplayerDetail::multiplayerMux());
#endif
    if (hasPayload_) {
        length = payloadLength_ < maxLength ? payloadLength_ : maxLength;
        memcpy(buffer, payload_, length);
        if (type) {
            *type = payloadType_;
        }
        if (from) {
            *from = payloadFrom_;
        }
        hasPayload_ = false;
    }
#if defined(ARDUINO_ARCH_ESP32)
    portEXIT_CRITICAL(&PoomMultiplayerDetail::multiplayerMux());
#endif
    return length;
}

inline void PoomMultiplayer::clear()
{
#if defined(ARDUINO_ARCH_ESP32)
    portENTER_CRITICAL(&PoomMultiplayerDetail::multiplayerMux());
#endif
    peerPlayer_ = 0;
    peerId_ = 0;
    lastReceivedSequence_ = 0;
    lastSeenMs_ = 0;
    receivedButtons_ = 0;
    hasPeer_ = false;
    hasPayload_ = false;
    payloadType_ = 0;
    payloadLength_ = 0;
    payloadFrom_ = 0;
#if defined(ARDUINO_ARCH_ESP32)
    portEXIT_CRITICAL(&PoomMultiplayerDetail::multiplayerMux());
#endif
    remoteButtons_ = 0;
    remotePreviousButtons_ = 0;
    remoteJustPressed_ = 0;
    remoteJustReleased_ = 0;
}

inline uint32_t PoomMultiplayer::localId() const { return localId_; }
inline uint32_t PoomMultiplayer::peerId() const { return peerId_; }
inline uint8_t PoomMultiplayer::localPlayer() const { return localPlayer_; }
inline uint8_t PoomMultiplayer::peerPlayer() const { return peerPlayer_; }
inline uint8_t PoomMultiplayer::channel() const { return channel_; }
inline uint32_t PoomMultiplayer::lastSeen() const { return lastSeenMs_; }
inline bool PoomMultiplayer::lastSendSucceeded() const { return lastSendSucceeded_; }

inline bool PoomMultiplayer::sendPacket(
    uint8_t type,
    const void *payload,
    uint8_t length,
    uint8_t buttons
)
{
#if defined(ARDUINO_ARCH_ESP32)
    if (!enabled_ || length > POOM_MULTIPLAYER_MAX_PAYLOAD ||
        (length > 0 && !payload)) {
        return false;
    }

    PoomMultiplayerDetail::PoomMultiplayerPacket packet = {};
    packet.magic = PoomMultiplayerDetail::PoomMultiplayerMagic;
    packet.version = PoomMultiplayerDetail::PoomMultiplayerVersion;
    packet.type = type;
    packet.player = localPlayer_;
    packet.buttons = buttons;
    packet.gameHash = gameHash_;
    packet.senderId = localId_;
    packet.sequence = ++sequence_;
    packet.payloadLength = length;
    if (length > 0) {
        memcpy(packet.payload, payload, length);
    }

    const size_t packetLength =
        offsetof(PoomMultiplayerDetail::PoomMultiplayerPacket, payload) + length;
    return esp_now_send(
        PoomMultiplayerDetail::BroadcastAddress,
        reinterpret_cast<const uint8_t *>(&packet),
        packetLength
    ) == ESP_OK;
#else
    (void)type;
    (void)payload;
    (void)length;
    (void)buttons;
    return false;
#endif
}

inline void PoomMultiplayer::receivePacket(const uint8_t *data, int length)
{
    const size_t headerLength =
        offsetof(PoomMultiplayerDetail::PoomMultiplayerPacket, payload);
    if (!data || length < static_cast<int>(headerLength)) {
        return;
    }

    const PoomMultiplayerDetail::PoomMultiplayerPacket *packet =
        reinterpret_cast<const PoomMultiplayerDetail::PoomMultiplayerPacket *>(data);
    if (packet->magic != PoomMultiplayerDetail::PoomMultiplayerMagic ||
        packet->version != PoomMultiplayerDetail::PoomMultiplayerVersion ||
        packet->gameHash != gameHash_ ||
        packet->senderId == localId_ ||
        packet->payloadLength > POOM_MULTIPLAYER_MAX_PAYLOAD ||
        headerLength + packet->payloadLength > static_cast<size_t>(length)) {
        return;
    }

    const uint32_t now = millis();
#if defined(ARDUINO_ARCH_ESP32)
    portENTER_CRITICAL(&PoomMultiplayerDetail::multiplayerMux());
#endif
    const bool activeOtherPeer = hasPeer_ && peerId_ != packet->senderId &&
        static_cast<uint32_t>(now - lastSeenMs_) <= POOM_MULTIPLAYER_STALE_MS;
    const bool restartingPeer = packet->type == PacketTypeHello;
    const bool staleSequence = hasPeer_ && peerId_ == packet->senderId &&
        !restartingPeer && !sequenceIsNewer(packet->sequence, lastReceivedSequence_);

    if (!activeOtherPeer && !staleSequence) {
        peerId_ = packet->senderId;
        peerPlayer_ = packet->player;
        lastReceivedSequence_ = packet->sequence;
        lastSeenMs_ = now;
        receivedButtons_ = packet->buttons;
        hasPeer_ = true;

        if (packet->type >= PacketTypeUser && packet->payloadLength > 0) {
            payloadType_ = packet->type;
            payloadLength_ = packet->payloadLength;
            payloadFrom_ = packet->senderId;
            memcpy(payload_, packet->payload, payloadLength_);
            hasPayload_ = true;
        }
    }
#if defined(ARDUINO_ARCH_ESP32)
    portEXIT_CRITICAL(&PoomMultiplayerDetail::multiplayerMux());
#endif
}

inline bool PoomMultiplayer::addBroadcastPeer()
{
#if defined(ARDUINO_ARCH_ESP32)
    if (esp_now_is_peer_exist(PoomMultiplayerDetail::BroadcastAddress)) {
        return true;
    }

    esp_now_peer_info_t peer = {};
    memcpy(
        peer.peer_addr,
        PoomMultiplayerDetail::BroadcastAddress,
        sizeof(peer.peer_addr)
    );
    peer.channel = channel_;
    peer.ifidx = WIFI_IF_STA;
    peer.encrypt = false;
    const esp_err_t result = esp_now_add_peer(&peer);
    return result == ESP_OK || result == ESP_ERR_ESPNOW_EXIST;
#else
    return false;
#endif
}

inline uint32_t PoomMultiplayer::hashGameId(const char *gameId)
{
    uint32_t hash = 2166136261UL;
    while (gameId && *gameId) {
        hash ^= static_cast<uint8_t>(*gameId++);
        hash *= 16777619UL;
    }
    return hash == 0 ? 1 : hash;
}

inline uint32_t PoomMultiplayer::makeLocalId()
{
#if defined(ARDUINO_ARCH_ESP32)
    uint8_t mac[6] = {0};
    if (esp_read_mac(mac, ESP_MAC_WIFI_STA) == ESP_OK) {
        return (static_cast<uint32_t>(mac[2]) << 24U) |
            (static_cast<uint32_t>(mac[3]) << 16U) |
            (static_cast<uint32_t>(mac[4]) << 8U) |
            static_cast<uint32_t>(mac[5]);
    }
#endif
    return static_cast<uint32_t>(micros());
}

inline bool PoomMultiplayer::sequenceIsNewer(uint32_t incoming, uint32_t previous)
{
    return static_cast<int32_t>(incoming - previous) > 0;
}

#if defined(ARDUINO_ARCH_ESP32)
inline void PoomMultiplayer::onReceive(
    const esp_now_recv_info_t *info,
    const uint8_t *data,
    int length
)
{
    (void)info;
    if (PoomMultiplayerDetail::activeMultiplayer()) {
        PoomMultiplayerDetail::activeMultiplayer()->receivePacket(data, length);
    }
}

inline void PoomMultiplayer::onSent(
    const esp_now_send_info_t *info,
    esp_now_send_status_t status
)
{
    (void)info;
    if (PoomMultiplayerDetail::activeMultiplayer()) {
        PoomMultiplayerDetail::activeMultiplayer()->lastSendSucceeded_ =
            status == ESP_NOW_SEND_SUCCESS;
    }
}
#endif
