#pragma once
#include "../BaseIncludes.h"
#include "SDK-Addition/ThreadSafeVar.h"
#include "mathlib/mathlib.h"
#include <deque>

#define MAX_CHOKED_HISTORY 128 // 2 Seconds cause we aint on 128 tick servers lol




class Entity;
class PlayerRecord
{
public:
	PlayerRecord() { m_dqChokedHistory.clear(); }
	PlayerRecord(Entity*);

	//PlayerRecord& operator=(PlayerRecord& other);
	void Reset()
	{
		m_muChokedHistoryMutex.lock();
		m_muGlobalWriteMutex.lock();
		m_nChokedTicks = 0;
		m_pEntity = nullptr;
		m_bWasDormant = false;
		m_nUserID = 0;
		m_nLastSetupBones = 0;
		m_nLastUpdate = 0;
		m_dqChokedHistory.clear();
		m_muChokedHistoryMutex.unlock();
		m_muGlobalWriteMutex.unlock();
	}
	void Init(Entity* pEnt);
	int LastSetupBones() { return m_nLastSetupBones; }
	int LastUpdate() { return m_nLastUpdate; }
	void BonesSetup(int nTickCount) { m_nLastSetupBones = nTickCount; };
	void Update();
	void OnDormancyEnter() { m_muChokedHistoryMutex.lock();  m_dqChokedHistory.clear(); m_muChokedHistoryMutex.unlock(); m_bWasDormant = true; }
	void OnDormancyExit(){ m_bWasDormant = false; }
	__forceinline int GetUserID() { return m_nUserID; };
	__forceinline Entity* GetEntity() { return m_pEntity; };

	void Lock() { m_muGlobalWriteMutex.lock(); }
	bool TryLock() { return m_muGlobalWriteMutex.try_lock(); }
	void UnLock() { m_muGlobalWriteMutex.try_lock(); }
	LockInstance AquireLockInstance() { return LockInstance(&m_muGlobalWriteMutex); }
	// Debug
	std::string RenderChokedHistroyToString(int nLength = 8);


	void SetInThreadedBonesSetup(bool bVal, int nTickCount)
	{
		m_muThreadedBonesMutex.lock();
		m_nLastAttemptedBonesSetup = nTickCount;
		m_bInThreadedSetup = bVal;
		m_muThreadedBonesMutex.unlock();
	}

	bool IsInThreadedBonesSetup()
	{
		return m_bInThreadedSetup;
	}

	bool __declspec(noinline) ShouldAttemptedThreadedBoneSetup(int nTickcount)
	{
		return !m_bInThreadedSetup && (m_nLastAttemptedBonesSetup < nTickcount);
	}


	matrix3x4a_t* GetStoredBones()
	{
		return m_pBonesSave;
	}

	//ThreadSafeVar<int> m_nChokedPackets = 0;
	int m_nChokedTicks = 0;
	int m_nLastUpdate = 0;
	std::deque<int> m_dqChokedHistory;
	int m_nLastUpdatedTickCount{ 0 };
	float m_flLastSimeTime = 0.f;
	bool m_bWasDormant = false;
	Vector m_vecLastNetworkedOrigin{ 0,0,0 };
	Vector m_vecLastNetworkedVelocity{ 0,0,0 };


private:
	int m_nUserID = -1;
	Entity* m_pEntity = nullptr;

	std::mutex m_muChokedHistoryMutex;
	std::mutex m_muGlobalWriteMutex;
	std::mutex m_muThreadedBonesMutex;
	int m_nLastAttemptedBonesSetup = 0;
	bool m_bInThreadedSetup = false;
	int m_nLastSetupBones{ 0 };
	matrix3x4a_t m_pBonesSave[256];
};


class PlayerRecords 
{
public:
	PlayerRecord* GetPlayerRecord(int nIndex);
private:
	PlayerRecord m_arrRecords[65];
};

inline PlayerRecords g_Records;
inline PlayerRecords* g_pRecords = &g_Records;
