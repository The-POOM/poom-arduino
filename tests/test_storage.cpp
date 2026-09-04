#include <assert.h>
#include <stdint.h>

#include "Preferences.h"
#include "PoomStorage.h"

struct SaveRecord
{
    uint32_t score;
    uint8_t level;
    uint8_t reserved[3];
};

int main()
{
    Preferences::clearAll();

    PoomStorage storage;
    assert(storage.begin("game", false, "game_nvs"));
    assert(storage.writeByte("level", 4));
    assert(storage.readByte("level") == 4);
    assert(storage.writeUInt32("score", 123456));
    assert(storage.readUInt32("score") == 123456);

    const SaveRecord saved = {98765, 7, {0, 0, 0}};
    assert(storage.writeRecord("progress", 2, &saved, sizeof(saved)));

    SaveRecord loaded = {};
    assert(storage.readRecord("progress", 2, &loaded, sizeof(loaded)));
    assert(loaded.score == saved.score);
    assert(loaded.level == saved.level);

    SaveRecord wrongVersion = {};
    assert(!storage.readRecord("progress", 3, &wrongVersion, sizeof(wrongVersion)));
    assert(Preferences::corrupt("game_nvs", "game", "progress"));
    assert(!storage.readRecord("progress", 2, &loaded, sizeof(loaded)));
    storage.end();

    PoomStorage otherPartition;
    assert(otherPartition.begin("game", false, "other_nvs"));
    assert(!otherPartition.contains("level"));

    return 0;
}
