#include "../BaseIncludes.h"
#include "../Memory.h"
#include "SDK.h"
#include "CStudioHdr.h"
#include "SDK-Addition/AnimationToString.h"
#include "SDK-Addition/Records/PlayerRecord.h"
#include "../FunctionTypeDeclares.h"
#include "tier0/threadtools.h"

int Entity::GetUserID()
{
	if (player_info_t playerInfo; g_pInterfaces->m_pEngine->GetPlayerInfo(EntIndex(), &playerInfo))
		return playerInfo.userId;

	return -1;
}


bool Entity::IsBot() {
	if (player_info_t playerInfo; g_pInterfaces->m_pEngine->GetPlayerInfo(EntIndex(), &playerInfo))
		return playerInfo.fakeplayer;

	return false;
}

bool Entity::IsLocalPlayer()
{
	return this == g_pLocalPlayer.Get();
}

CBoneAccessor* Entity::GetBoneAccessor()
{
	return (CBoneAccessor*)((char*)this + (int)9888);//*(CBoneAccessor**)((char*)this + *(short*)g_pMemory->GetAddress(__BONEACCESSOR));
}


__forceinline C_AnimationLayer* Entity::GetAnimationLayer(int overlay) noexcept {
	return &AnimOverlays()[overlay];
}

__forceinline C_AnimationLayer* Entity::GetAnimOverlay(int overlay) noexcept {
	return &AnimOverlays()[overlay];
}

bool Entity::Teleported()
{
	return ((TeleportedFunc_t)g_pMemory->GetAddress(__TELEPORTED))(this);
}





CStudioHdr* Entity::GetModelPtr()
{
#if 1
	_asm int 3 // This Offset Is Wrong!
	CStudioHdr* hdr = *(CStudioHdr**)((*(int*)g_pMemory->GetAddress(__CBASEENTITY_M_P_STUDIOHDR)) + (char*)this);
	if (!hdr && GetModel())
	{
		
		BaseEntityLockStudioHdr(this);
	}
	return (hdr && hdr->IsValid()) ? hdr : NULL;


#endif
}



studiohdr_t* Entity::GetStudioHdr()
{
	return g_pInterfaces->m_pModelInfo->GetStudioModel(GetModel());
}

bool Entity::CorrectSetupBones(int nBoneMask, float flCurrentTime)
{



	return false;
}

int Entity::GetSequenceActivity(int sequence)
{
	studiohdr_t* pStudioHdr = g_pInterfaces->m_pModelInfo->GetStudioModel(GetModel());

	if (!pStudioHdr)
		return 0;

	static GetSequenceActivityFunc_t GetSequenceAct{ reinterpret_cast<GetSequenceActivityFunc_t>(g_pMemory->GetAddress(__GETSEQUENCEACTIVITY)) };
	return  GetSequenceAct(this, pStudioHdr, sequence);
}

void Entity::DrawAnimationDebug(int& nDrawPos)
{
	DrawSequenceActivities(nDrawPos);
}

void Entity::DrawDebugOverlay()
{
	static IVDebugOverlay* pOverlay = g_pInterfaces->m_pDebugOverlay;

	int nDrawPosition = 0;

	pOverlay->AddEntityTextOverlay(EntIndex(), nDrawPosition, 9999.f, 255, 255, 255, 255, " Entity Index : %d", EntIndex());
	nDrawPosition++;

	pOverlay->AddEntityTextOverlay(EntIndex(), nDrawPosition, 9999.f, 255, 255, 255, 255, " Simulation Time : %f", m_flSimulationTime());
	nDrawPosition++;

	if (IsPlayer())
	{
		pOverlay->AddEntityTextOverlay(EntIndex(), nDrawPosition, 9999.f, 255, 255, 255, 255, " Choked History : %s", GetPlayerRecord()->RenderChokedHistroyToString(8).c_str());
		nDrawPosition++;

		DrawAnimationDebug(nDrawPosition);
	}
}

void Entity::DrawSequenceActivities(int& nDrawPos)
{

#ifndef _LIVE
	try {
		static IVDebugOverlay* pOverlay = g_pInterfaces->m_pDebugOverlay;

		//pOverlay->AddEntityTextOverlay(EntIndex(), -10, 9999.f, 255, 255, 255, 255, " Sequence Activity : %d", GetAnim);
		for (int b = 0; b < GetAnimationLayerCount(); b++) {

			C_AnimationLayer* AnimLayer = GetAnimationLayer(b);

			if (!AnimLayer->IsActive())
				continue;


			if (!AnimLayer)
				continue;

			const model_t* pModel = GetModel();
			if (!pModel)
				continue;

			studiohdr_t* pStudioHdr = g_pInterfaces->m_pModelInfo->GetStudioModel(pModel);
			if (!pStudioHdr)
				continue;

			int nActivity = GetSequenceActivity(AnimLayer->m_nSequence);

			if (nActivity == -1)
				continue;

			// strings at end to it doesnt jump everywhere
			pOverlay->AddEntityTextOverlay(EntIndex(), nDrawPos, 9999.f, 255, 255, 255, 255, " Sequence Activity %02d : %03d  - Player Back : %f - Weight : %f - Cycle : %f - (%s) - (%s)", b, nActivity, AnimLayer->m_flPlaybackRate, AnimLayer->m_flWeight, AnimLayer->m_flCycle, AnimationActToString(nActivity).c_str(), AnimationLayerToString(b).c_str());
			nDrawPos++;
		}
	}
	catch (std::exception(&e))
	{
		DEBUG_OUT_STR(e.what());
	}
#else
	CON("Called Draw Sequence Activities, But Compiled For Live?");
#endif
}

float DotProductT(const float* a, const float* b)
{
	return (a[0] * b[0] + a[1] * b[1] + a[2] * b[2]);
}

void VectorTransformA(const float* in1, const matrix3x4_t& in2, float* out)
{

	out[0] = DotProductT(in1, in2[0]) + in2[0][3];
	out[1] = DotProductT(in1, in2[1]) + in2[1][3];
	out[2] = DotProductT(in1, in2[2]) + in2[2][3];
}


void inline VectorTransformZ(const Vector& in1, const matrix3x4_t& in2, Vector& out)
{
	VectorTransformA(&in1.x, in2, &out.x);
}

QAngle CalcAngle(Vector& src, Vector& dst)
{
	QAngle angles;
	VectorAngles(dst - src, angles);

	return angles;
}

void Entity::GetBoneTransformed(matrix3x4_t* srcMatrix, mstudiobbox_t* hitbox, matrix3x4_t* rotatedmatrix, float* radius, Vector* minsmaxs, Vector* worldminsmaxs)
{
	if (minsmaxs)
	{
		minsmaxs[0] = hitbox->bbmin;
		minsmaxs[1] = hitbox->bbmax;
	}

	if (radius)
		*radius = hitbox->radius;

	if (hitbox->radius != -1.0f)
	{
		//capsule
		if (rotatedmatrix)
			*rotatedmatrix = srcMatrix[hitbox->bone];

		if (worldminsmaxs)
		{
			//TransformAABB(srcMatrix[hitbox->bone], hitbox->bbmin, hitbox->bbmax, worldminsmaxs[0], worldminsmaxs[1]);
			VectorTransformZ(hitbox->bbmin, srcMatrix[hitbox->bone], worldminsmaxs[0]);
			VectorTransformZ(hitbox->bbmax, srcMatrix[hitbox->bone], worldminsmaxs[1]);

			// get hitbox center point
			Vector forward, right, up;

			// get forward from Hitbox
			QAngle AngleFromMinsMaxs = CalcAngle(worldminsmaxs[0], worldminsmaxs[1]);

			AngleVectors(AngleFromMinsMaxs, &forward, &right, &up);
			VectorNormalizeFast(forward);
			VectorNormalizeFast(right);
			VectorNormalizeFast(up);

			float _radius = hitbox->radius;

			// get adjusted points
			auto _adjust = (forward * _radius) + (right * _radius) + (up * _radius);
			worldminsmaxs[0] -= _adjust; //bottom
			worldminsmaxs[1] += _adjust; //top
		}
	}
	else
	{
		//box
		if (rotatedmatrix || worldminsmaxs)
		{
			matrix3x4_t matrix_rotation;
			matrix3x4_t final_matrix;
			AngleMatrix(hitbox->angles, matrix_rotation);
			ConcatTransforms(srcMatrix[hitbox->bone], matrix_rotation, final_matrix);

			if (rotatedmatrix)
				*rotatedmatrix = final_matrix;

			if (worldminsmaxs)
			{
				TransformAABB(final_matrix, hitbox->bbmin, hitbox->bbmax, worldminsmaxs[0], worldminsmaxs[1]);
			}
		}
	}
}

// Stolen Mutiny Code Cause It Works Real Well
bool Entity::ComputeHitboxSurroundingBox(Vector* pVecWorldMins, Vector* pVecWorldMaxs, matrix3x4_t* pSourceMatrix, bool use_valve)
{
#if 1
	studiohdr_t* pStudioHdr = GetStudioHdr();
	if (!pStudioHdr)
		return false;

	mstudiohitboxset_t* set = pStudioHdr->pHitboxSet(m_nHitboxSet());
	if (!set || !set->numhitboxes)
		return false;

	matrix3x4_t* hitboxbones = pSourceMatrix;
	if (!pSourceMatrix)
	{
		auto cache = GetBoneAccessor()->GetBoneArrayForWrite();
		hitboxbones = (matrix3x4_t*)cache->Base();
	}

	// Compute a box in world space that surrounds this entity
	pVecWorldMins->Init(FLT_MAX, FLT_MAX, FLT_MAX);
	pVecWorldMaxs->Init(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	if (!use_valve)
	{
		// sharklaser's fix for valve's broken version of this function
		for (int i = 0; i < set->numhitboxes; i++)
		{
			mstudiobbox_t* pbox = set->pHitbox(i);
			Vector _minsmaxs[2];
			GetBoneTransformed(hitboxbones, pbox, nullptr, nullptr, nullptr, _minsmaxs);
			pVecWorldMins->x = fmin(pVecWorldMins->x, _minsmaxs[0].x);
			pVecWorldMins->y = fmin(pVecWorldMins->y, _minsmaxs[0].y);
			pVecWorldMins->z = fmin(pVecWorldMins->z, _minsmaxs[0].z);
			pVecWorldMaxs->x = fmax(pVecWorldMaxs->x, _minsmaxs[1].x);
			pVecWorldMaxs->y = fmax(pVecWorldMaxs->y, _minsmaxs[1].y);
			pVecWorldMaxs->z = fmax(pVecWorldMaxs->z, _minsmaxs[1].z);
		}
	}
	else
	{
		Vector vecBoxAbsMins, vecBoxAbsMaxs;

		for (int i = 0; i < set->numhitboxes; i++)
		{
			mstudiobbox_t* pbox = set->pHitbox(i);

			TransformAABB(hitboxbones[pbox->bone], pbox->bbmin, pbox->bbmax, vecBoxAbsMins, vecBoxAbsMaxs);
			pVecWorldMins->x = fmin(pVecWorldMins->x, vecBoxAbsMins.x);
			pVecWorldMins->y = fmin(pVecWorldMins->y, vecBoxAbsMins.y);
			pVecWorldMins->z = fmin(pVecWorldMins->z, vecBoxAbsMins.z);
			pVecWorldMaxs->x = fmax(pVecWorldMaxs->x, vecBoxAbsMaxs.x);
			pVecWorldMaxs->y = fmax(pVecWorldMaxs->y, vecBoxAbsMaxs.y);
			pVecWorldMaxs->z = fmax(pVecWorldMaxs->z, vecBoxAbsMaxs.z);
		}
	}

	return true;
#endif
	return false;
}


PlayerRecord* Entity::GetPlayerRecord()
{
	return g_pRecords->GetPlayerRecord(EntIndex());
}


bool Entity::SetupBonesForThread()
{
	return CallVirtualFunction<bool, matrix3x4_t*, int, int, float>(ICLIENTRENDERABLE, 13, nullptr, 256, 256, gpGlobals->m_flCurtime);
}

typedef void(__thiscall* InvalidateBoneCacheFunc_t)(Entity*);
void Entity::InvalidateBoneCache()
{
	static InvalidateBoneCacheFunc_t _InvalideBoneCache{ reinterpret_cast<InvalidateBoneCacheFunc_t>(g_pMemory->GetAddress(__INVALIDATEBONECACHE)) };
	_InvalideBoneCache(this);
}
struct ThreadProcInfo_t
{
	ThreadProcInfo_t(ThreadFunc_t pfnThread, void* pParam)
		: pfnThread(pfnThread),
		pParam(pParam)
	{
	}

	ThreadFunc_t pfnThread;
	void* pParam;
};

uintp _cdecl CallSetupBones(Entity* pEnt)
{
	PlayerRecord* pRecord = pEnt->GetPlayerRecord();
	if (!pRecord)
		return 0;

	pRecord->SetInThreadedBonesSetup(true, gpGlobals->m_nTickcount);
	pEnt->InvalidateBoneCache();
	CallVirtualFunction<bool, matrix3x4a_t*, int, int, float>(pEnt + 4, 13, pRecord->GetStoredBones(), 256, 256, gpGlobals->m_flCurtime);
	pRecord->SetInThreadedBonesSetup(false, gpGlobals->m_nTickcount);
	return 0;
}