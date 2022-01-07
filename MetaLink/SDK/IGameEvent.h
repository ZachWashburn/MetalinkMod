#pragma once

#include <stdint.h>
#include "ents_shared.h"

class KeyValues;
class bf_read;
class bf_write;
class KeyValues;
class CGameEvent;
class CSVCMsg_GameEvent;
class SendTable;
class ClientClass;

// Class for visiting every key of data on an event
class IGameEventVisitor2
{
public:
	// return true to keep iterating, false to abort iteration
	virtual bool VisitLocal(const char* name, const void* value) { return true; }
	virtual bool VisitString(const char* name, const char* value) { return true; }
	virtual bool VisitFloat(const char* name, float value) { return true; }
	virtual bool VisitInt(const char* name, int value) { return true; }
	virtual bool VisitUint64(const char* name, uint64_t value) { return true; }
	virtual bool VisitWString(const char* name, const wchar_t* value) { return true; }
	virtual bool VisitBool(const char* name, bool value) { return true; }
};


class IGameEvent
{
public:
	virtual ~IGameEvent() {};
	virtual const char* GetName() const = 0;	// get event name
	virtual bool  IsReliable() const = 0; // if event handled reliable
	virtual bool  IsLocal() const = 0; // if event is never networked
	virtual bool  IsEmpty(const char* keyName = 0) const = 0; // check if data field exists
	// Data access
	virtual bool  GetBool(const char* keyName = 0, bool defaultValue = false) const = 0;
	virtual int   GetInt(const char* keyName = 0, int defaultValue = 0) const = 0;
	virtual uint64_t GetUint64(const char* keyName = 0, uint64_t defaultValue = 0) const = 0;
	virtual float GetFloat(const char* keyName = 0, float defaultValue = 0.0f) const = 0;
	virtual const char* GetString(const char* keyName = 0, const char* defaultValue = "") const = 0;
	virtual const wchar_t* GetWString(const char* keyName = 0, const wchar_t* defaultValue = L"") const = 0;
	virtual const void* GetPtr(const char* keyName = 0) const = 0;	// LOCAL only
	virtual void SetBool(const char* keyName, bool value) = 0;
	virtual void SetInt(const char* keyName, int value) = 0;
	virtual void SetUint64(const char* keyName, uint64_t value) = 0;
	virtual void SetFloat(const char* keyName, float value) = 0;
	virtual void SetString(const char* keyName, const char* value) = 0;
	virtual void SetWString(const char* keyName, const wchar_t* value) = 0;
	virtual void SetPtr(const char* keyName, const void* value) = 0;	// LOCAL only
	virtual bool ForEventData(IGameEventVisitor2* event) const = 0;
};

class IGameEventListener
{
public:
	virtual	~IGameEventListener(void) {};
	// FireEvent is called by EventManager if event just occured
	// KeyValue memory will be freed by manager if not needed anymore
	virtual void FireGameEvent(KeyValues* event) = 0;
};


class IGameEventListener2
{
public:
	virtual	~IGameEventListener2(void) {};

	// FireEvent is called by EventManager if event just occured
	// KeyValue memory will be freed by manager if not needed anymore
	virtual void FireGameEvent(IGameEvent* event) = 0;

	virtual int	 GetEventDebugID(void) = 0;
};


class IGameEventManager {
public:
	virtual	~IGameEventManager(void) {};
	// load game event descriptions from a file eg "resource\gameevents.res"
	virtual int LoadEventsFromFile(const char* filename) = 0;
	// removes all and anything
	virtual void  Reset() = 0;
	virtual KeyValues* GetEvent(const char* name) = 0; // returns keys for event
	// adds a listener for a particular event
	virtual bool AddListener(IGameEventListener* listener, const char* event, bool bIsServerSide) = 0;
	// registers for all known events
	virtual bool AddListener(IGameEventListener* listener, bool bIsServerSide) = 0;
	// removes a listener 
	virtual void RemoveListener(IGameEventListener* listener) = 0;
	// fires an global event, specific event data is stored in KeyValues
	// local listeners will receive the event instantly
	// a network message will be send to all connected clients to invoke
	// the same event there
	virtual bool FireEvent(KeyValues* event) = 0;
	// fire a side server event, that it wont be broadcasted to player clients
	virtual bool FireEventServerOnly(KeyValues* event) = 0;
	// fires an event only on this local client
	// can be used to fake events coming over the network 
	virtual bool FireEventClientOnly(KeyValues* event) = 0;
	// write/read event to/from bitbuffer
	virtual bool SerializeKeyValues(KeyValues* event, bf_write* buf, CGameEvent* eventtype = NULL) = 0;
	virtual KeyValues* UnserializeKeyValue(bf_read* msg) = 0; // create new KeyValues, must be deleted
};


class CEventInfo
{
public:
	enum
	{
		EVENT_INDEX_BITS = 8,
		EVENT_DATA_LEN_BITS = 11,
		MAX_EVENT_DATA = 192,  // ( 1<<8 bits == 256, but only using 192 below )
	};

	inline CEventInfo()	{	}

	~CEventInfo()
	{
		//g_pSerializedEntities->ReleaseSerializedEntity(m_Packed);
	}

	CEventInfo(const CEventInfo& src)	{	}

	// 0 implies not in use
	short m_nClassID;

	// If non-zero, the delay time when the event should be fired ( fixed up on the client )
	float m_flFireDelay;

	// send table pointer or NULL if send as full update
	const SendTable* pSendTable;
	const ClientClass* pClientClass;

	int						m_nBits;
	uint8_t* m_pData;
	int						m_nFlags;
	PAD(0x16);
	CEventInfo* m_pNext;

	// clients that see that event
	//CEngineRecipientFilter filter;
};