#include "PlayerRecord.h"
#include "SDK.h"

void PlayerRecord::Init(Entity* pEnt)
{
	m_muGlobalWriteMutex.lock();
	m_muChokedHistoryMutex.lock();
	m_dqChokedHistory.clear();
	m_nUserID = pEnt->GetUserID();
	m_pEntity = pEnt;
	m_muChokedHistoryMutex.unlock();
	m_muGlobalWriteMutex.unlock();
}

PlayerRecord::PlayerRecord(Entity* pEnt)
{
	Init(pEnt);
}

void PlayerRecord::Update()
{

	if (!m_pEntity)
		return;

	LockInstance CurrentLock = LockInstance(&m_muGlobalWriteMutex);

	m_nLastUpdate = gpGlobals->m_nTickcount;

	bool bDormant = m_pEntity->IsDormant();
	if (bDormant && !m_bWasDormant)
	{
		OnDormancyEnter();
	}
	else if (bDormant && m_bWasDormant)
	{
		return;
	}
	else if (m_bWasDormant)
	{
		OnDormancyExit();
	}

	if (m_flLastSimeTime == m_pEntity->m_flSimulationTime()) {
		if(!m_pEntity->IsBot())
			m_nChokedTicks = g_pInterfaces->m_pGlobalVars->m_nTickcount - m_nLastUpdatedTickCount;
		return;
	}

	m_flLastSimeTime = m_pEntity->m_flSimulationTime();

	if (m_pEntity->IsBot())
	{
		return;
	}

	m_nLastUpdatedTickCount = g_pInterfaces->m_pGlobalVars->m_nTickcount;

	m_muChokedHistoryMutex.lock();
	m_dqChokedHistory.push_front(m_nChokedTicks);
	while (m_dqChokedHistory.size() > MAX_CHOKED_HISTORY)
		m_dqChokedHistory.pop_back();
	m_muChokedHistoryMutex.unlock();


	m_nChokedTicks = 0;

	m_vecLastNetworkedOrigin = m_pEntity->m_vecNetworkOrigin();
	m_vecLastNetworkedVelocity = m_pEntity->m_vecVelocity();

	//VCON("Updating Player Record for Entity %02d!, simtime %04f\n", m_pEntity->entindex(), m_flLastSimeTime);
	return;
}

std::string PlayerRecord::RenderChokedHistroyToString(int nLength)
{
	if(!m_muChokedHistoryMutex.try_lock())
		return std::string("NO LOCK");

	std::string strOut = "";
	int nLast = min(nLength, m_dqChokedHistory.size());
	for (int i = 0; i < nLast; i++)
	{
		strOut += std::to_string(m_dqChokedHistory.at(i));
		if (i+1 < nLast) strOut += std::string("-");
	}
	m_muChokedHistoryMutex.unlock();
	return strOut;
}


PlayerRecord* PlayerRecords::GetPlayerRecord(int nIndex)
{
	if ((nIndex < 0) || (nIndex > g_pInterfaces->m_pEngine->GetMaxClients())) {
		DEBUG_OUT_STR("GetPlayerRecord Called With Invalid Index?");
		return nullptr;
	}

	PlayerRecord* pRecord = &(m_arrRecords[nIndex]);

	Entity* pEnt = g_pInterfaces->m_pEntityList->GetClientEntity(nIndex);
	if (!pEnt)
	{
		m_arrRecords[nIndex].Reset();
		return nullptr;
	}

	if ((pEnt->GetUserID() != pRecord->GetUserID()) || (pEnt != pRecord->GetEntity()))
	{
		m_arrRecords[nIndex].Reset();
		m_arrRecords[nIndex].Init(pEnt);
	}

	return pRecord;
}

