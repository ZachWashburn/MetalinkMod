#pragma once
#include "../BaseIncludes.h"
#include "mathlib/mathlib.h"

class INetChannel;
class CEventInfo;

class CBaseClientState
{
public:
	void ForceFullUpdate()
	{
		m_nDeltaTick = -1;
	}
	char		pad0[0x9C];
	INetChannel* m_pNetChannel;
	int				m_nChallengeNr;
	std::byte		pad1[0x04];
	double          m_dbConnectTime;
	int             m_nRetryNumber;
	std::byte		padgay[0x54];
	int				m_nSignonState;
	std::byte		pad2[0x8];
	float           m_flNextCmdTime;
	int				m_nServerCount;
	int				m_nCurrentSequence;
	char			pad99[8];
	struct {
		float		m_flClockOffsets[16];
		int			m_nCurClockOffset;
		int			m_nServerTick;
		int			m_nClientTick;
	} m_clock_drift_mgr;
	int				m_nDeltaTick;
	bool			m_bPaused;
	std::byte		pad4[0x7];
	int				m_nViewEntity;
	int				m_nPlayerSlot;
	char			m_szLevelName[260];
	char			m_szLevelNameShort[80];
	char			m_cGroupName[80];
	char			m_szLastLevelNameShort[80]; // 0x032C
	std::byte		pad5[0x8];
	int				m_nNumPlayersToConnect;
	int				m_nMaxClients;
	char pad_030C[4083];
	uint32_t m_pStringTableContainer;
	char pad_1303[14737];
	float			m_flLastServerTickTime;
	bool			m_bInSimulation;
	std::byte		pad7[0x3];
	int				m_nOldTickcount;
	float			m_flTickRemainder;
	float			m_flFrameTime;
	int				m_nLastOutgoingCommand;
	int				m_nChokedCommands;
	int				m_nLastCommandAck;
	int				m_nCommandAck;
	int				m_nSoundSequence;
	//std::byte		pad8[0x50];
	int                m_nLastProgressPercent;
	bool            m_bIsHltv;

	std::byte padfuck[0x4B];
	Vector			m_vecAngViewPoint;
	std::byte		pad9[0xD0];
	CEventInfo* m_pEvents;
};