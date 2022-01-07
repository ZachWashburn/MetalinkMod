#pragma once

#include "BaseIncludes.h"
#include <map>
struct AntColor {
    AntColor()
    {
        color.at(0) = 1.f;
        color.at(1) = 1.f;
        color.at(2) = 1.f;
        color.at(3) = 1.f;
    }

    std::array<float, 4> color;
    const std::array<float, 4>* operator->() const {
        return &color;
    }
    __forceinline float& fr() { return color.at(0); }
    __forceinline float& fg() { return color.at(1); }
    __forceinline float& fb() { return color.at(2); }
    __forceinline float& fa() { return color.at(3); }
    __forceinline int r() { return static_cast<int>(color.at(0) * 255.f); }
    __forceinline int g() { return static_cast<int>(color.at(1) * 255.f); }
    __forceinline int b() { return static_cast<int>(color.at(2) * 255.f); }
    __forceinline int a() { return static_cast<int>(color.at(3) * 255.f); }
};

struct ColorToggle {
    AntColor color;
    bool bEnabled{ false };
};

class Config {
public:

    struct AimbotWeapon {
        enum ParseMode {
            NOT_SET,
            BEST_DAMAGE,
            NEAREST_POINT,
        };
        bool bEnabled{ false };
        bool bSilent{ false };
        bool bAutoShoot{ false };
        bool bAutoScope{ false };
        int nHitChance{ 0 };
        ParseMode m_nParseMode{ NOT_SET };
        int nMinDamage{ 0 };
    };

    struct Aimbot {
        AimbotWeapon arrWeapons[40];
    } m_Aimbot;


    struct Cham_s {
        struct Material {
            bool bEnabled{ false };
            bool bRenderInSceneEnd{ false };
            bool bCallOriginalInSceneEnd{ false };
            std::string MaterialName;
            AntColor color;
            struct Options {
                bool bEnabled = false;
                bool bHealthBased = false;
                bool bBlinking = false;
                bool bWireframe = false;
                bool bCover = false;
                bool bIgnorez = false;
                int nMaterial = 0;
                struct {
                    bool enabled = false;
                    float rate = .1f;
                    float a = 1.0f;
                } pulse;
            } Opts;
        };
        bool bEnabled{ true };
        bool bCallOriginal{ false };
        Material Materials[10];
    };
    std::map <uint32_t, Cham_s> m_mapChams;



    struct ESP {
        struct EntityESP {
            struct Glow {
                ColorToggle m_Color;
                int m_nGlowStyle{ 0 };
                float m_fGlowMaxBrightness{ 0.6f };
            } m_Glow;
            bool bEnabled{ false };
            ColorToggle ctBox;
            ColorToggle ctName;
            struct EntityDebugESP {
                ColorToggle ctVelocity;
                ColorToggle ctOrigin;
                ColorToggle ctEntIndex;
            } m_DebugEsp;
        };
        struct PlayerESP : EntityESP {
            uint32_t m_nSteamID{ 0 }; // For Checking to see if theres a new player in that slot
            ColorToggle ctWeapon;
            Cham_s Cham; // Unused For EnemyESP and TeammateESP
        } m_arrPlayerESP[64]; /* For Custom Player ESP Shit */
        PlayerESP m_EnemyESP;
        PlayerESP m_TeammateESP;
        bool m_bEnabled = true;
    } m_ESP;


    struct AntiAim {
        bool bEnabled{ true };
        bool bMicroMove{ false };
        bool bLBY{ false };




    } m_AntiAim;




    struct Networking {
        struct misc {
            bool bForceSpectate{ false };
        } m_Misc;
    } m_Networking;


    struct Debug {
        bool bAimbotDebug{ true };

    } m_Debug;


    struct Backtrack {
        bool bEnabled{ false };
        bool bFakeLatencyEnabled{ false };
        int nMaxTime{ 0 };



        struct TickShiftExtension {
            bool bEnabled{ false };
        } m_TickShiftBT;
    } m_Backtrack;



#ifdef EXPLOITS
    struct Exploits {
        bool bAllowUntrusted{ false };
        struct TickBase {
            struct DoubleTap {
                bool bEnabled{ false };
                bool bHideShots{ false };
                bool bClMoveDT{ true };
                bool bHostRunFrameInputDT{ false };
                bool bWriteUserCmdtoDeltaDT{ false };
                int nCustomMaxUserProcess{ 0 };
                int nTicksToShift{ 8 };
                bool bInstaRecharge{ false };
            }m_DoubleTap;
        } m_TickBase;
    } m_Exploits;

#ifdef INCLUDE_PUBLIC_CRASHER
    struct ServerLaggers {
        struct LZSSUncompressLagger {
            bool bEnabled{ false };
            int nPacketSize{ 10 };
            int nSubChannelPacketSize{ 22 };
            bool bSkipOnCount{ true };
            bool bReliableOnly{ false };
            bool bOnVoice{ false };
            int nCountToSkip{ 1 };
            int nWaitTime{ 0 };
            int nActiveTime{ 0 };
        } m_LZSSLagger;
    } m_ServerLaggers;

#endif // INCLUDE_PUBLIC_CRASHER


#ifdef GAME_BREAK_EXPLOITS
#endif // GAME_BREAK_EXPLOITS


#endif // EXPLOITS


#ifdef DEV_SECRET_BUILD
#ifdef _DEBUG

#endif // _DEBUG
#endif // DEV_SECRET_BUILD


private:
};

inline Config g_ConfigNotToBeUsedEver;

// Make IDA reversal just ever so more annoying
class ConfigGetter {
public:
    ConfigGetter() {}
    __declspec(noinline) Config* operator->() {
        return GetConfig();
    }
    __declspec(noinline) ConfigGetter(Config* config) : Conf(config) {}
    __declspec(noinline) Config* GetConfig() {
        return Conf;
    }
    __declspec(noinline) void SetConfig(Config* config) {
        Conf = config;
    }
private:
    Config* Conf{ nullptr };
};
inline ConfigGetter g_pConfig;


__declspec(noinline) void SetConfigToGetter(Config* conf);