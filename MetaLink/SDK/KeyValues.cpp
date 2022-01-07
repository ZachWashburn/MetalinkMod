#include "KeyValues.h"
#include "../Memory.h"


typedef KeyValues* (__thiscall* KeyValuesFindKeyFunc_t)(KeyValues*, const char*, bool);
typedef void(__thiscall* KeyValuesSetStringFunc_t)(KeyValues*, const char*);
typedef const char* (__thiscall* KeyValuesGetNameFunc_t)(void*);
typedef int(__thiscall* KeyValuesGetIntFunc_t)(void*, const char*, int);
typedef const char* (__thiscall* KeyValuesGetStringFunc_t)(void*, const char*, const char*);
typedef float(__thiscall* KetValuesGetFloatFunc_t)(void*, const char*, float);
KeyValuesFindKeyFunc_t kvFindKey = NULL;
KeyValuesSetStringFunc_t kvSetString = NULL;
KeyValuesGetNameFunc_t kvGetName = NULL;
KeyValuesGetIntFunc_t kvGetInt = NULL;
KeyValuesGetStringFunc_t kvGetString = NULL;
KetValuesGetFloatFunc_t kvGetFloat = NULL;
LPVOID kvFromString = NULL;



void SetupKeyValues()
{
    kvFromString = MemoryTools::FindPattern(__CLIENT, XorStr("\xE8????\x83\xC4\x04\x89\x45\xD8"), 1, DEBUGSTR("KeyValues::FromString"));
    kvFindKey = reinterpret_cast<KeyValuesFindKeyFunc_t>(MemoryTools::FindPattern(__CLIENT, XorStr("\xE8????\xF7\x45"), 1, DEBUGSTR("KeyValues::FindKey")));
    kvSetString = reinterpret_cast<KeyValuesSetStringFunc_t>(MemoryTools::FindPattern(__CLIENT, XorStr("\xE8????\x89\x77\x38"), 1, DEBUGSTR("KeyValues::SetString")));
    kvGetName = reinterpret_cast<KeyValuesGetNameFunc_t>(MemoryTools::FindPattern(__ENGINE, XorStr("\xE8????\x8D\x50\x01"), 1, DEBUGSTR("KeyValues::GetName")));
    kvGetInt = reinterpret_cast<KeyValuesGetIntFunc_t>(MemoryTools::FindPattern(__MATCHMAKING, XorStr("\xE8????\x6A\x04"), 1, DEBUGSTR("KeyValues::GetInt")));
    kvGetString = reinterpret_cast<KeyValuesGetStringFunc_t>(MemoryTools::FindPattern(__MATCHMAKING, XorStr("\xE8????\x89\x45\x18"), 1, DEBUGSTR("KeyValues::GetString")));
    kvGetFloat = reinterpret_cast<KetValuesGetFloatFunc_t>(MemoryTools::FindPattern(__MATCHMAKING, XorStr("\xE8????\x8B\x06\x8D\x4D\x08"), 1, DEBUGSTR("KeyValues::GetFloat")));
}





KeyValues* KeyValues::FromString(const char* name, const char* value) noexcept
{

    KeyValues* keyValues;
    __asm {
        push 0
        mov edx, value
        mov ecx, name
        call kvFromString
        add esp, 4
        mov keyValues, eax
    }
    return keyValues;
}

KeyValues* KeyValues::FindKey(const char* keyName, bool create) noexcept
{
    return kvFindKey(this, keyName, create);
}

void KeyValues::SetString(const char* keyName, const char* value) noexcept
{
    if (const auto key = FindKey(keyName, true))
        kvSetString(key, value);
}

const char* KeyValues::GetName() noexcept
{
    return kvGetName(this);
}

int KeyValues::GetInt(const char* keyName, int defaultValue)
{
    return kvGetInt(this, keyName, defaultValue);
}

const char* KeyValues::GetString(const char* keyName, const char* defaultValue)
{
    return kvGetString(this, keyName, defaultValue);
}

float KeyValues::GetFloat(const char* keyName, float defaultValue)
{
    return kvGetFloat(this, keyName, defaultValue);
}

