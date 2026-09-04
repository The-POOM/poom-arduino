#ifndef TEST_PREFERENCES_H
#define TEST_PREFERENCES_H

#include <map>
#include <string>
#include <vector>

#include <stddef.h>
#include <stdint.h>
#include <string.h>

class Preferences
{
public:
    bool begin(const char *name, bool readOnly = false, const char *partitionLabel = nullptr)
    {
        if (!name || !partitionLabel) {
            return false;
        }
        namespace_ = std::string(partitionLabel) + "/" + name;
        readOnly_ = readOnly;
        open_ = true;
        return true;
    }

    void end()
    {
        open_ = false;
    }

    bool isKey(const char *key)
    {
        return find(key) != values().end();
    }

    uint8_t getUChar(const char *key, uint8_t defaultValue = 0)
    {
        const ValueMap::const_iterator entry = find(key);
        return entry != values().end() && entry->second.size() == sizeof(uint8_t)
            ? entry->second[0]
            : defaultValue;
    }

    uint32_t getULong(const char *key, uint32_t defaultValue = 0)
    {
        const ValueMap::const_iterator entry = find(key);
        uint32_t value = defaultValue;
        if (entry != values().end() && entry->second.size() == sizeof(value)) {
            memcpy(&value, entry->second.data(), sizeof(value));
        }
        return value;
    }

    size_t putUChar(const char *key, uint8_t value)
    {
        return putBytes(key, &value, sizeof(value));
    }

    size_t putULong(const char *key, uint32_t value)
    {
        return putBytes(key, &value, sizeof(value));
    }

    size_t getBytesLength(const char *key)
    {
        const ValueMap::const_iterator entry = find(key);
        return entry == values().end() ? 0 : entry->second.size();
    }

    size_t getBytes(const char *key, void *value, size_t maxLength)
    {
        const ValueMap::const_iterator entry = find(key);
        if (entry == values().end() || !value || entry->second.size() > maxLength) {
            return 0;
        }
        memcpy(value, entry->second.data(), entry->second.size());
        return entry->second.size();
    }

    size_t putBytes(const char *key, const void *value, size_t length)
    {
        if (!open_ || readOnly_ || !key || !value || length == 0) {
            return 0;
        }
        const uint8_t *bytes = static_cast<const uint8_t *>(value);
        values()[qualifiedKey(key)] = std::vector<uint8_t>(bytes, bytes + length);
        return length;
    }

    static void clearAll()
    {
        values().clear();
    }

    static bool corrupt(const char *partitionLabel, const char *name, const char *key)
    {
        const std::string qualified = std::string(partitionLabel) + "/" + name + "/" + key;
        ValueMap::iterator entry = values().find(qualified);
        if (entry == values().end() || entry->second.empty()) {
            return false;
        }
        entry->second.back() ^= 0xFFU;
        return true;
    }

private:
    typedef std::map<std::string, std::vector<uint8_t> > ValueMap;

    bool open_ = false;
    bool readOnly_ = false;
    std::string namespace_;

    static ValueMap &values()
    {
        static ValueMap storedValues;
        return storedValues;
    }

    std::string qualifiedKey(const char *key) const
    {
        return namespace_ + "/" + (key ? key : "");
    }

    ValueMap::iterator find(const char *key)
    {
        if (!open_ || !key) {
            return values().end();
        }
        return values().find(qualifiedKey(key));
    }
};

#endif
