
#include "Animations.h"
#include "SDK.h"
#if 0
void C_CSGOPlayerAnimState::SetupLean()
{
	//START_PROFILING
#ifdef _DEBUG
	((void(__thiscall*)(C_CSGOPlayerAnimState*))StaticOffsets.GetOffsetValue(_SetupLean))(this);
	//END_PROFILING
		return;
#endif

	float timedelta = Interfaces::Globals->curtime - m_flLastSetupLeanCurtime;
	if (timedelta > 0.025f)
	{
		if (timedelta >= 0.1f)
			timedelta = 0.1f;

		Vector vecVelocity = pBaseEntity->GetVelocity();
		m_flLastSetupLeanCurtime = Interfaces::Globals->curtime;
		m_vecSetupLeanVelocityDelta.x = (vecVelocity.x - m_vecLastSetupLeanVelocity.x) * (1.0f / timedelta);
		m_vecSetupLeanVelocityDelta.y = (vecVelocity.y - m_vecLastSetupLeanVelocity.y) * (1.0f / timedelta);
		m_vecSetupLeanVelocityDelta.z = 0.0f;
		m_vecLastSetupLeanVelocity = vecVelocity;
	}

	m_vecSetupLeanVelocityInterpolated = g_LagCompensation.GetSmoothedVelocity(m_flLastClientSideAnimationUpdateTimeDelta * 800.0f, m_vecSetupLeanVelocityDelta, m_vecSetupLeanVelocityInterpolated);

	Vector pseudoup = Vector(0.0f, 0.0f, 1.0f);
	QAngle angles;
	VectorAngles(m_vecSetupLeanVelocityInterpolated, pseudoup, angles);

	float speed = (m_vecSetupLeanVelocityInterpolated.Length() * (1.0f / _CS_PLAYER_SPEED_RUN)) * m_flSpeedNormalized;

	//*(float *)&animstate->m_bNotRunning ON SERVER
	m_flLeanWeight = (1.0f - m_flLadderCycle) * clamp(speed, 0.0f, 1.0f);

	float flGoalFeetLeanDelta = AngleNormalize(m_flGoalFeetYaw - angles.y);

	m_arrPoseParameters[0].SetValue(pBaseEntity, flGoalFeetLeanDelta);

	C_AnimationLayer* layer12 = pBaseEntity->GetAnimOverlay(12);
	if (!layer12 || layer12->_m_nSequence <= 0)
	{
		Interfaces::MDLCache->BeginLock();
		//decrypts(0)
		int seq = pBaseEntity->LookupSequence(XorStr("lean"));
		//encrypts(0)
		SetLayerSequence(12, seq);
		Interfaces::MDLCache->EndLock();
	}

	SetLayerWeight(12, m_flLeanWeight);

	//Other shit is done here on the server
	END_PROFILING
}
#endif