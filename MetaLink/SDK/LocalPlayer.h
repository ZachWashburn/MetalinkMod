#pragma once

#include <cassert>

class Entity;

class LocalPlayer {
public:
    void init(Entity** entity) noexcept
    {
        pLocalEntity = entity;
    }

    constexpr operator bool() noexcept
    {
        //assert(localEntity);
        return *pLocalEntity != nullptr;
    }

    constexpr auto operator->() noexcept
    {
        //assert(localEntity && *localEntity);
        return *pLocalEntity;
    }

    constexpr auto Get() noexcept
    {
        //assert(localEntity && *localEntity);
        return *pLocalEntity;
    }
    bool m_bRunningFakeAngleBones = false;
private:
    Entity** pLocalEntity = nullptr;

};

inline LocalPlayer g_pLocalPlayer;
