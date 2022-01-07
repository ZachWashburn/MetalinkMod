#pragma once
#include "../BaseIncludes.h"
#include "VirtualFunction.h"
#include "IClientNetworkable.h"
#include "IClientRenderable.h"
#include "CBoneAccessor.h"
#include "Netvars.h"
#include "../FunctionTypeDeclares.h"

#define ICLIENTRENDERABLE (this+4)
#define ICLIENTNETWORKABLE (this+8)

#define BASEENTITY_INCLUDE_NETWORKABLE
#define BASEENTITY_INCLUDE_RENDERABLE

#define	LIFE_ALIVE				0 // alive
#define	LIFE_DYING				1 // playing death animation or still falling off of a ledge waiting to hit ground
#define	LIFE_DEAD				2 // dead. lying still.
#define LIFE_RESPAWNABLE		3
#define LIFE_DISCARDBODY		4


#define ENTCLIENTFLAG_GETTINGSHADOWRENDERBOUNDS	0x0001		// Tells us if we're getting the real ent render bounds or the shadow render bounds.
#define ENTCLIENTFLAG_DONTUSEIK					0x0002		// Don't use IK on this entity even if its model has IK.
#define ENTCLIENTFLAG_ALWAYS_INTERPOLATE		0x0004		// Used by view models.


class ClientClass;
class C_AnimationLayer;
class CStudioHdr;
class C_AnimationLayer;
class WeaponInfo;
class PlayerRecord;
struct mstudiobbox_t;

enum class MoveType {
	MOVETYPE_NONE = 0,	// never moves
	MOVETYPE_ISOMETRIC,			// For players -- in TF2 commander view, etc.
	MOVETYPE_WALK,				// Player only - moving on the ground
	MOVETYPE_STEP,				// gravity, special edge handling -- monsters use this
	MOVETYPE_FLY,				// No gravity, but still collides with stuff
	MOVETYPE_FLYGRAVITY,		// flies through the air + is affected by gravity
	MOVETYPE_VPHYSICS,			// uses VPHYSICS for simulation
	MOVETYPE_PUSH,				// no clip to world, push and crush
	NOCLIP = 8,
	LADDER = 9,
	MOVETYPE_NOCLIP = 8,
	MOVETYPE_LADDER = 9,
	MOVETYPE_OBSERVER,			// Observer movement, depends on player's observer mode
	MOVETYPE_CUSTOM,			// Allows the entity to describe its own physics
	// should always be defined as the last item in the list
	MOVETYPE_LAST = MOVETYPE_CUSTOM,
	MOVETYPE_MAX_BITS = 4
};


enum entity_list_ids_t
{
	ENTITY_LIST_INTERPOLATE = 0,
	ENTITY_LIST_TELEPORT,
	ENTITY_LIST_PRERENDER,
	ENTITY_LIST_SIMULATE,
	ENTITY_LIST_DELETE,

	NUM_ENTITY_LISTS
};;

enum class Team {
	None = 0,
	Spectators,
	TT,
	CT
};

// entity effects
enum
{
	EF_BONEMERGE = 0x001,	// Performs bone merge on client side
	EF_BRIGHTLIGHT = 0x002,	// DLIGHT centered at entity origin
	EF_DIMLIGHT = 0x004,	// player flashlight
	EF_NOINTERP = 0x008,	// don't interpolate the next frame
	EF_NOSHADOW = 0x010,	// Don't cast no shadow
	EF_NODRAW = 0x020,	// don't draw entity
	EF_NORECEIVESHADOW = 0x040,	// Don't receive no shadow
	EF_BONEMERGE_FASTCULL = 0x080,	// For use with EF_BONEMERGE. If this is set, then it places this ent's origin at its
										// parent and uses the parent's bbox + the max extents of the aiment.
										// Otherwise, it sets up the parent's bones every frame to figure out where to place
										// the aiment, which is inefficient because it'll setup the parent's bones even if
										// the parent is not in the PVS.
										EF_ITEM_BLINK = 0x100,	// blink an item so that the user notices it.
										EF_PARENT_ANIMATES = 0x200,	// always assume that the parent entity is animating
										EF_MARKED_FOR_FAST_REFLECTION = 0x400,	// marks an entity for reflection rendering when using $reflectonlymarkedentities material variable
										EF_NOSHADOWDEPTH = 0x800,	// Indicates this entity does not render into any shadow depthmap
										EF_SHADOWDEPTH_NOCACHE = 0x1000,	// Indicates this entity cannot be cached in shadow depthmap and should render every frame
										EF_NOFLASHLIGHT = 0x2000,
										EF_NOCSM = 0x4000,	// Indicates this entity does not render into the cascade shadow depthmap
										EF_MAX_BITS = 15
};

class SINGLE_INHERITANCE Entity {
public:
	/* Client Networkable */
	ClassFunction IClientNetworkable* GetIClientNetworkable() { return reinterpret_cast<IClientNetworkable*>(ICLIENTNETWORKABLE); }
#ifdef BASEENTITY_INCLUDE_NETWORKABLE
	ClassFunction IClientUnknown* GetIClientUnknown() { return CallVirtualFunction<IClientUnknown*>(ICLIENTNETWORKABLE, 0); }
	ClassFunction void Release() { return CallVirtualFunction<void>(ICLIENTNETWORKABLE, 1); }
	ClassFunction ClientClass* GetClientClass() { return CallVirtualFunction<ClientClass*>(ICLIENTNETWORKABLE, 2); }
	ClassFunction void NotifyShouldTransmit(ShouldTransmitState_t nState) { return CallVirtualFunction<void, ShouldTransmitState_t>(ICLIENTNETWORKABLE, 3, nState); }
	ClassFunction void OnPreDataChanged(DataUpdateType_t nUpdateType) { return CallVirtualFunction<void, DataUpdateType_t>(ICLIENTNETWORKABLE, 4, nUpdateType); }
	ClassFunction void OnPostDataChanged(DataUpdateType_t nUpdateType) { return CallVirtualFunction<void, DataUpdateType_t>(ICLIENTNETWORKABLE, 5, nUpdateType); }
	ClassFunction void PreDataUpdate(int nUpdateType) { return CallVirtualFunction<void, int>(ICLIENTNETWORKABLE, 6, nUpdateType); }
	ClassFunction void PostDataUpdate(int nUpdateType) { return CallVirtualFunction<void, int>(ICLIENTNETWORKABLE, 7, nUpdateType); }
	ClassFunction void OnDataUnchangedInPVS() { return CallVirtualFunction<void>(ICLIENTNETWORKABLE, 8); }
	ClassFunction bool IsDormant() { return CallVirtualFunction<bool>(ICLIENTNETWORKABLE, 9); }
	ClassFunction int  EntIndex() { return CallVirtualFunction<bool>(ICLIENTNETWORKABLE, 10); }
	ClassFunction int  Index() { return CallVirtualFunction<bool>(ICLIENTNETWORKABLE, 10); }
	ClassFunction int  entindex() { return CallVirtualFunction<bool>(ICLIENTNETWORKABLE, 10); } // Lowercase just for easier rebuilding. just ignore it aight?
	ClassFunction void ReceiveMessage(int nClassID, bf_read& bfrMsg) { return CallVirtualFunction<void, int, bf_read&>(ICLIENTNETWORKABLE, 11, nClassID, bfrMsg); }
	ClassFunction void* GetTableBasePtr() { return CallVirtualFunction<void*>(ICLIENTNETWORKABLE, 12); } // Get the base pointer to the networked data that GetClientClass->m_pRecvTable starts at.
	ClassFunction void SetDestroyedOnRecreateEntites() { return CallVirtualFunction<void>(ICLIENTNETWORKABLE, 13); }
#endif

	/* Client Renderable */
	ClassFunction IClientRenderable* GetIClientRenderable() { return reinterpret_cast<IClientRenderable*>(ICLIENTRENDERABLE); }
#ifdef BASEENTITY_INCLUDE_RENDERABLE
	ClassFunction IClientUnknown* GetIClientUnknown2() { return CallVirtualFunction<IClientUnknown*>(ICLIENTRENDERABLE, 0); } // Just for completion sake
	ClassFunction Vector const& GetRenderOrigin() { return CallVirtualFunction<Vector&>(ICLIENTRENDERABLE, 1); }
	ClassFunction Vector const& GetRenderAngles() { return CallVirtualFunction<Vector&>(ICLIENTRENDERABLE, 2); } // If you're using this and its still Vector& make it QAngle&
	ClassFunction bool ShouldDraw()    { return CallVirtualFunction<bool>(ICLIENTRENDERABLE, 3); }
	ClassFunction int GetRenderFlags() { return CallVirtualFunction<int>(ICLIENTRENDERABLE, 4); }
	ClassFunction void Unused() const {};
	ClassFunction ClientShadowHandle_t GetShadowHandle() { return CallVirtualFunction<ClientShadowHandle_t>(ICLIENTRENDERABLE, 6); }
	ClassFunction ClientRenderHandle_t RenderHandle() { return CallVirtualFunction<ClientRenderHandle_t>(ICLIENTRENDERABLE, 7); }
	ClassFunction const model_t* GetModel() { return CallVirtualFunction<model_t*>(ICLIENTRENDERABLE, 8); }
	ClassFunction void DrawModel(int nFlags, const RenderableInstance_t& pInstance) { return CallVirtualFunction<void, int, RenderableInstance_t>(ICLIENTRENDERABLE, 9, nFlags, std::ref(pInstance)); }
	ClassFunction int GetBody() { return CallVirtualFunction<int>(ICLIENTRENDERABLE, 10); }
	ClassFunction bool LODTest() { return CallVirtualFunction<bool>(ICLIENTRENDERABLE, 11); }
	ClassFunction bool SetupBones(matrix3x4_t* pBoneToWorldOut, int nMaxBones, int nBoneMask, float flCurrentTime) { return CallVirtualFunction<bool, matrix3x4_t*, int, int, float>(ICLIENTRENDERABLE, 13, pBoneToWorldOut, nMaxBones, nBoneMask, flCurrentTime); }
	ClassFunction void SetupWeights(const matrix3x4_t* pBoneToWorld, int nFlexWeightCount, float* pFlexWeights, float* pFlexDelayedWeights) { return CallVirtualFunction<void, const matrix3x4_t*, int, float*, float*>(ICLIENTRENDERABLE, 14, pBoneToWorld, nFlexWeightCount,pFlexWeights,pFlexDelayedWeights); }
	ClassFunction void DoAnimationEvents() { return CallVirtualFunction<void>(ICLIENTRENDERABLE, 15); }
	ClassFunction IPVSNotify* GetPVSNotifyInterface() { return CallVirtualFunction<IPVSNotify*>(ICLIENTRENDERABLE, 16); }
	ClassFunction void GetRenderBounds(Vector& vMins, Vector& vMaxs) { return CallVirtualFunction<void, Vector&, Vector&>(ICLIENTRENDERABLE, 17, vMins, vMaxs); }
	ClassFunction void GetRenderBoundsWorldSpace(Vector& vMins, Vector& vMaxs) { return CallVirtualFunction<void, Vector&, Vector&>(ICLIENTRENDERABLE, 18, std::ref(vMins), std::ref(vMaxs)); }
	ClassFunction void GetShadowRenderBounds(Vector& vMins, Vector& vMaxs, ShadowType_t nShadowType) { return CallVirtualFunction<void, Vector&, Vector&, ShadowType_t>(ICLIENTRENDERABLE, 19, std::ref(vMins), std::ref(vMaxs), nShadowType); }
	ClassFunction bool ShouldReceiveProjectedTextures(int nFlags) { return CallVirtualFunction<bool, int>(ICLIENTRENDERABLE, 20, nFlags); }
	ClassFunction void GetShadowCastDistance(float* pDist, ShadowType_t nShadowType) { return CallVirtualFunction<void, float*, ShadowType_t>(ICLIENTRENDERABLE, 21, pDist, nShadowType); }
	ClassFunction void Unused2() const {}; // From Here to 32 it seems like shits changed. I'll fill these in later
	FUNC_PLACEHOLDER(CreateModelInstance); // 22 <- I think this is this function, but i didnt 100% verify
	FUNC_PLACEHOLDER(); // 23
	FUNC_PLACEHOLDER(); // 24
	FUNC_PLACEHOLDER();
	FUNC_PLACEHOLDER();
	FUNC_PLACEHOLDER();
	FUNC_PLACEHOLDER();
	FUNC_PLACEHOLDER();
	FUNC_PLACEHOLDER();
	FUNC_PLACEHOLDER(); // 31
	ClassFunction const matrix3x4_t& RenderableToWorldTransform() { return CallVirtualFunction<matrix3x4_t&>(ICLIENTRENDERABLE, 32); }

#endif


	/* BaseEntity */

	ClassFunction ICollideable* GetCollideable() { return CallVirtualFunction < ICollideable*>(this, 3); }
	ClassFunction Vector& GetAbsOrigin() { return CallVirtualFunction<Vector&>(this, 10); }
	ClassFunction Vector& GetAbsAngle() { return CallVirtualFunction<Vector&>(this, 11); }
	ClassFunction Vector* GetAbsOriginPtr() { return CallVirtualFunction<Vector*>(this, 10); }
	ClassFunction Vector* GetAbsAnglePtr() { return CallVirtualFunction<Vector*>(this, 11); }
	ClassFunction void SetModelIndex(int nIndex) { return CallVirtualFunction<void, int>(this, 75, nIndex); }
	ClassFunction bool InitializeAsClientEntity(const char* szModelName, int nRenderGroup) { return CallVirtualFunction<bool, const char*, int>(this, 96, szModelName, nRenderGroup); }
	ClassFunction int Health() { return CallVirtualFunction<int>(this, 121); }
	ClassFunction bool IsAlive() { return !m_lifeState(); }
	ClassFunction bool IsPlayer() { return CallVirtualFunction<bool>(this, 157); } 
	ClassFunction bool IsWeapon() { return CallVirtualFunction<bool>(this, 165); } 

	ClassFunction void ControlMouth(CStudioHdr* hdr) { return CallVirtualFunction<void, CStudioHdr*>(this, 196, hdr); }
	ClassFunction bool DoExtraBoneProcessing(CStudioHdr* hdr, Vector* pos, Quaternion* q, matrix3x4_t* bonearray, CBoneBitList* computed, CIKContext* m_pIK){ return CallVirtualFunction<bool, CStudioHdr*, Vector*, Quaternion*, matrix3x4_t*, CBoneBitList*, CIKContext*>(this, 197, hdr, pos, q, bonearray, computed, m_pIK); }
	


	ClassFunction int GetWeaponType() {
		return CallVirtualFunction<int>(this, 454);
	}

	ClassFunction WeaponInfo* GetWeaponInfo() {
		return CallVirtualFunction<WeaponInfo*>(this, 460);
	}


	
	//void StandardBlendingRules(CStudioHdr* hdr, Vector* pPos, BoneQuaternionAligned* qa, float curTime, int bonemask);
	NETVAR(m_flSunDistance, "CSunlightShadowControl", "m_flSunDistance", float)
	NETVAR(m_vecVelocity, "CBasePlayer", "m_vecVelocity[0]", Vector)

	NETVAR(m_nBody, "CBaseAnimating", "m_nBody", int)
	NETVAR(m_nHitboxSet, "CBaseAnimating", "m_nHitboxSet", int)
	NETVAR(m_flCycle, "CBaseAnimating", "m_flCycle", float);
	NETVAR(m_nSequence, "CBaseAnimating", "m_flCycle", int);
	NETVAR(m_flModelScale, "CBaseAnimating", "m_flModelScale", float);
	NETVAR(m_bClientSideRagdoll, "CBaseAnimating", "m_bClientSideRagdoll", bool);

	NETVAR(m_nModelIndex, "CBaseEntity", "m_nModelIndex", unsigned)
	NETVAR(m_vecOrigin, "CBaseEntity", "m_vecOrigin", Vector)
	NETVAR_OFFSET(m_nMoveType, "CBaseEntity", "m_nRenderMode", 1, MoveType)
	NETVAR(m_flSimulationTime, "CBaseEntity", "m_flSimulationTime", float)
	NETVAR(m_hOwnerEntity, "CBaseEntity", "m_hOwnerEntity", int)
	NETVAR(m_iTeamNum, "CBaseEntity", "m_iTeamNum", int)
	NETVAR(m_bSpotted, "CBaseEntity", "m_bSpotted", bool)
	NETVAR_OFFSET(m_rgflCoordinateFrame, "m_CollisionGroup", "m_rgflCoordinateFrame", -0x30, matrix3x4)
	NETVAR(m_vecNetworkOrigin, "CBaseEntity", "m_vecNetworkOrigin", Vector)
	NETVAR(m_iCollisionGroup, "CBaseEntity", "m_CollisionGroup", int)

	NETVAR(m_usSolidFlags, "CBaseEntity", "m_usSolidFlags", int)
	NETVAR(m_nSolidType, "CBaseEntity ", "m_nSolidType ", int)

	NETVAR(m_iItemDefinitionIndex, "CBaseAttributableItem", "m_iItemDefinitionIndex", short)


	NETVAR(m_hMyWeapons, "CBaseCombatCharacter", "m_hMyWeapons", int[64])
	PNETVAR(m_hMyWearables, "CBaseCombatCharacter", "m_hMyWearables", int)


	NETVAR(m_hViewModel, "CBasePlayer", "m_hViewModel[0]", int)
	NETVAR(m_iFOV, "CBasePlayer", "m_iFOV", int)
	NETVAR(m_iFOVStart, "CBasePlayer", "m_iFOVStart", int)
	NETVAR(m_iDefaultFOV, "CBasePlayer", "m_iDefaultFOV", int)
	NETVAR(m_fFlags, "CBasePlayer", "m_fFlags", int)
	NETVAR(m_nTickBase, "CBasePlayer", "m_nTickBase", int)
	NETVAR(m_aimPunchAngle, "CBasePlayer", "m_aimPunchAngle", Vector)
	NETVAR(m_viewPunchAngle, "CBasePlayer", "m_viewPunchAngle", Vector)
	NETVAR(m_vecViewOffset, "CBasePlayer", "m_vecViewOffset[0]", Vector);
	NETVAR(m_lifeState, "CBasePlayer", "m_lifeState", int32_t);
	NETVAR(m_fEffects, "CBasePlayer", "m_fEffects", int);

	NETVAR(m_szLastPlaceName, "CBasePlayer", "m_szLastPlaceName", char[18])
	NETVAR(m_hObserverTarget, "CBasePlayer", "m_hObserverTarget", Entity*);
	NETVAR(m_iObserverMode, "CBasePlayer", "m_iObserverMode", int);

	NETVAR(m_ArmorValue, "CCSPlayer", "m_ArmorValue", int)
	NETVAR(m_angEyeAngles, "CCSPlayer", "m_angEyeAngles", Vector)
	NETVAR(m_bIsScoped, "CCSPlayer", "m_bIsScoped", bool)
	NETVAR(m_bIsDefusing, "CCSPlayer", "m_bIsDefusing", bool)
	NETVAR_OFFSET(m_flFlashDuration, "CCSPlayer", "m_flFlashMaxAlpha", -8, float)
	NETVAR(m_flFlashMaxAlpha, "CCSPlayer", "m_flFlashMaxAlpha", float)
	NETVAR(m_bGunGameImmunity, "CCSPlayer", "m_bGunGameImmunity", bool)
	NETVAR(m_iAccount, "CCSPlayer", "m_iAccount", int)
	NETVAR(m_bInBombZone, "CCSPlayer", "m_bInBombZone", bool)
	NETVAR(m_bHasDefuser, "CCSPlayer", "m_bHasDefuser", bool)
	NETVAR(m_bHasHelmet, "CCSPlayer", "m_bHasHelmet", bool)
	NETVAR(m_flLowerBodyYawTarget, "CCSPlayer", "m_flLowerBodyYawTarget", float)
	NETVAR(m_hRagdoll, "CCSPlayer", "m_hRagdoll", int)
	NETVAR(m_iShotsFired, "CCSPlayer", "m_iShotsFired", int)
	NETVAR(m_bWaitForNoAttack, "CCSPlayer", "m_bWaitForNoAttack", bool)

	NETVAR(m_iAccountID, "CBaseAttributableItem", "m_iAccountID", int)
	NETVAR(m_iItemIDHigh, "CBaseAttributableItem", "m_iItemIDHigh", int)
	NETVAR(m_iEntityQuality, "CBaseAttributableItem", "m_iEntityQuality", int)
	NETVAR(m_szCustomName, "CBaseAttributableItem", "m_szCustomName", char[32])
	NETVAR(m_nFallbackPaintKit, "CBaseAttributableItem", "m_nFallbackPaintKit", unsigned)
	NETVAR(m_nFallbackSeed, "CBaseAttributableItem", "m_nFallbackSeed", unsigned)
	NETVAR(m_flFallbackWear, "CBaseAttributableItem", "m_flFallbackWear", float)
	NETVAR(m_flFallbackStatTrak, "CBaseAttributableItem", "m_nFallbackStatTrak", unsigned)
	NETVAR(m_bInitialized, "CBaseAttributableItem", "m_bInitialized", bool)


	NETVAR(m_hViewModelOwner, "CBaseViewModel", "m_hOwner", int)
	NETVAR(m_hViewModelWeapon, "CBaseViewModel", "m_hWeapon", int)

	NETVAR(m_bStartedArming, "CC4", "m_bStartedArming", bool)

	NETVAR(m_bTabletReceptionIsBlocked, "CTablet", "m_bTabletReceptionIsBlocked", bool)

	NETVAR(m_hMoveToThisEntity, "CDrone", "m_hMoveToThisEntity", int)

	NETVAR(m_hThrower, "CBaseGrenade", "m_hThrower", int)

	NETVAR(m_bMapHasBombTarget, "CCSGameRulesProxy", "m_bMapHasBombTarget", bool)
	NETVAR(m_totalRoundsPlayed, "CCSGameRulesProxy", "m_totalRoundsPlayed", int)
	NETVAR(m_bWarmupPeriod, "CCSGameRulesProxy", "m_bWarmupPeriod", bool)
	NETVAR(m_bHasMatchStarted, "CCSGameRulesProxy", "m_bHasMatchStarted", bool)
	NETVAR(m_bFreezePeriod, "CCSGameRulesProxy", "m_bFreezePeriod", bool)
	NETVAR(m_bIsValveDS, "CCSGameRulesProxy", "m_bIsValveDS", bool)


	NETVAR_OFFSET(m_iCompetitiveRanking, "CCSPlayerResource", "m_iCompetitiveRanking", 4, int);
	NETVAR_OFFSET(m_iCompetitiveWins, "CCSPlayerResource", "m_iCompetitiveWins", 4, int);


	NETVAR(m_fireXDelta, "CInferno", "m_fireXDelta", int[100])
	NETVAR(m_fireYDelta, "CInferno", "m_fireYDelta", int[100])
	NETVAR(m_fireZDelta, "CInferno", "m_fireZDelta", int[100])
	NETVAR(m_bFireIsBurning, "CInferno", "m_bFireIsBurning", bool[100])
	NETVAR(m_fireCount, "CInferno", "m_fireCount", int)



	__forceinline bool IsPlayerBaseEntity() { int nIndex = EntIndex(); return (nIndex > 0) && (nIndex < 64); }
	Vector GetEyePosition() { Vector vPosition; CallVirtualFunction<void>(this, 284, std::ref(vPosition)); return vPosition; }
	Vector GetAimPunch() { Vector vPunch; CallVirtualFunction<void>(this, 345, std::ref(vPunch)); return vPunch; }
	int GetUserID();
	bool IsBot();

	// Animation Stuff

	bool CorrectSetupBones(int nBoneMask, float flCurrentTime);
	CBoneAccessor* GetBoneAccessor();
	void InvalidatePhysicsRecursive(int nChangeFlags) { throw; }
	int GetSequenceActivity(int sequence);
	__forceinline C_AnimationLayer* GetAnimationLayer(int overlay) noexcept;
	__forceinline C_AnimationLayer* GetAnimOverlay(int overlay) noexcept;
	CStudioHdr* GetModelPtr();
	bool Teleported();
	DEBUGNOINLINE void UpdateIKLocks(float currentTime) { 
		return CallVirtualFunction<void, float>(this, 191, currentTime); 
	};
	DEBUGNOINLINE void CalculateIKLocks(float currentTime) {
		return CallVirtualFunction<void, float>(this, 192, currentTime); 
	};
	void InvalidateBoneCache();
	// Stealing from shark cause it's actually genius
	bool ComputeHitboxSurroundingBox(Vector* pVecWorldMins, Vector* pVecWorldMaxs, matrix3x4_t* pSourceMatrix, bool use_valve = true);
	void GetBoneTransformed(matrix3x4_t* srcMatrix, mstudiobbox_t* hitbox, matrix3x4_t* rotatedmatrix, float* radius, Vector* minsmaxs, Vector* worldminsmaxs);


	bool SetupBonesForThread();

	studiohdr_t* GetStudioHdr();
	// Debug Stuff

	void DrawAnimationDebug(int& nDrawPos);
	void DrawDebugOverlay();
	void DrawSequenceActivities(int& nDrawPos);



	// Other
	PlayerRecord* GetPlayerRecord();
	bool IsLocalPlayer();

	// Anim

	__forceinline int GetAnimationLayerCount()
	{
#pragma message ("You need to not make this offset HardCoded!");
#ifdef _LIVE
		QuickCrash;
#endif
		return *reinterpret_cast<int*>(this + 0x298C);

	}

	__forceinline C_AnimationLayer* AnimOverlays()
	{
#pragma message ("You need to not make this offset HardCoded!");
#ifdef _LIVE
		QuickCrash;
#endif
		return *reinterpret_cast<C_AnimationLayer**>(uintptr_t(this) + 0x2980);
	}






private:
};

using CBaseEntity = Entity;
using C_BasePlayer = Entity;


inline LockStudioHdrFunc_t BaseEntityLockStudioHdr;


uintp _cdecl CallSetupBones(Entity* pEnt);