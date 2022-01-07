#pragma once
#include "../BaseIncludes.h"
#include "Interfaces.h"
#include "IBaseClientDLL.h"
#include "Entity.h"
#include "IClientEntityList.h"
#include "ICollideable.h"
#include "INetworkChannel.h"
#include "mathlib/mathlib.h"
#include "IVDebugOverlay.h"
#include "IGlobalVarsBase.h"
#include "IPrediction.h"
#include "CBaseClientState.h"
#include "PredictionDefines.h"
#include "CUserCmd.h"
#include "LocalPlayer.h"
#include "datamap.h"
#include "ISurface.h"
#include "IVModelInfo.h"
#include "IGameEvent.h"
#include "C_BaseAnimating.h"
#include "CIKContext.h"
#include "IFileSystem.h"
#include "IInputSystem.h"
#include "shareddefs.h"
#include "SDK-Addition/Records/PlayerRecord.h"
#include "SDK-Addition/DataStore.h"
#include "SDK-Addition/Threading.h"
#include "IStudioRender.h"
#include "IRenderView.h"
#include "IConvar.h"
#include "ICvar.h"
#include "IMatchFramework.h"
#include "tier1/convar.h"
#include "mathlib/vmatrix.h"
#include "IMaterial.h"
#include "IMaterialSystem.h"
#include "KeyValues.h"



// SteamNetworkingSockets
#include "SteamNetSockets/CSteamNetworkingMessage.h"

#define TICK_INTERVAL gpGlobals->m_flIntervalPerTick
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
#define TICKS_TO_TIME( t )		( TICK_INTERVAL *( t ) )
#define ROUND_TO_TICKS( t )		( TICK_INTERVAL * TIME_TO_TICKS( t ) )
#define TICK_NEVER_THINK		(-1)

class CBaseClientState;
namespace Globals {
	inline CBaseClientState* g_pClientState;
	inline Vector2D g_vecScreenSize;
    inline unsigned long long g_ServerReservationCookie = 0;
    inline ISteamUser* g_pSteamUser;
    inline ISteamNetworkingSockets* g_pSteamNetworkingSockets;
    inline ISteamNetworkingMessages* g_pSteamNetworkingMessages;
    inline ISteamNetworking* g_pSteamNetworking;
    inline ISteamNetworkingUtils* g_pSteamNetworkingUtils;
    inline ISteamHTTP* g_pSteamHTTP;
    inline ISteamUtils* g_pSteamUtils;
    inline ISteamMatchmaking* g_pSteamMatchmaking;
    inline ISteamFriends* g_pSteamFriends;
    inline std::uintptr_t g_pSteamApi;
    inline int g_nLastEncryptionKey = 0;
    inline int g_nLastChallengeNumber = 0;
    inline int g_nServerProtocolVersion = 0;
}
#define gpGlobals g_pInterfaces->m_pGlobalVars
#define gpClientState Globals::g_pClientState

// Lest Find A Better Place For This
// Also you can get fucked if you think i'm writing my own math functions
inline bool WorldToScreen(const Vector& vOrigin, Vector2D& vScreen)
{
    const auto& matrix = g_pDataStore->GetWorldToScreen();

    const auto w = matrix[3][0] * vOrigin.x + matrix[3][1] * vOrigin.y + matrix[3][2] * vOrigin.z + matrix[3][3];
    if (w < 0.001f)
        return false;

    int x, y = 0;
    g_pInterfaces->m_pSurface->GetScreenSize(x, y);

    Vector2D out = Vector2D{ (float)x, (float)y } / 2.f;
    out.x *= 1.0f + (matrix[0][0] * vOrigin.x + matrix[0][1] * vOrigin.y + matrix[0][2] * vOrigin.z + matrix[0][3]) / w;
    out.y *= 1.0f - (matrix[1][0] * vOrigin.x + matrix[1][1] * vOrigin.y + matrix[1][2] * vOrigin.z + matrix[1][3]) / w;

    vScreen = out;
    return true;


#if 0
    const VMatrix& worldToScreen = g_pInterfaces->m_pEngine->WorldToScreenMatrix();

    float w = worldToScreen[3][0] * vOrigin[0] + worldToScreen[3][1] * vOrigin[1] + worldToScreen[3][2] * vOrigin[2] + worldToScreen[3][3];
    if (w > 0.001)
    {
        float inverseWidth = 1 / w;
        vScreen.x = (Globals::g_vecScreenSize.x / 2) + (0.5 * ((worldToScreen[0][0] * vOrigin[0] + worldToScreen[0][1] * vOrigin[1] + worldToScreen[0][2] * vOrigin[2] + worldToScreen[0][3]) * inverseWidth) * Globals::g_vecScreenSize.x + 0.5);
        vScreen.y = (Globals::g_vecScreenSize.y / 2) - (0.5 * ((worldToScreen[1][0] * vOrigin[0] + worldToScreen[1][1] * vOrigin[1] + worldToScreen[1][2] * vOrigin[2] + worldToScreen[1][3]) * inverseWidth) * Globals::g_vecScreenSize.y + 0.5);
        return true;
    }
    return false;
#endif
}

inline bool WorldToScreen(const Vector& in, float& x, float& y)
{
    const auto& matrix = g_pDataStore->GetWorldToScreen();

    const auto w = matrix[3][0] * in.x + matrix[3][1] * in.y + matrix[3][2] * in.z + matrix[3][3];
    if (w < 0.001f)
        return false;

    int sx, sy = 0;
    g_pInterfaces->m_pSurface->GetScreenSize(sx, sy);

    Vector2D out = Vector2D{ (float)sx, (float)sy } / 2.f;
    x = out.x;
    y = out.y;
    x *= 1.0f + (matrix[0][0] * in.x + matrix[0][1] * in.y + matrix[0][2] * in.z + matrix[0][3]) / w;
    y *= 1.0f - (matrix[1][0] * in.x + matrix[1][1] * in.y + matrix[1][2] * in.z + matrix[1][3]) / w;

    return true;

}