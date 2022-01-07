//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
// $NoKeywords: $
//=============================================================================//
#if !defined( ICLIENTENTITYLIST_H )
#define ICLIENTENTITYLIST_H

#ifdef _WIN32
#pragma once
#endif

#include "../BaseIncludes.h"
#include "tier1/utlvector.h"
class IClientEntity;
class ClientClass;
class IClientNetworkable;
typedef uint32_t CBaseHandle;
class IClientUnknown;
class IClientRenderable;
class Entity;
#include "Entity.h"
// Cached info for networked entities.
// NOTE: Changing this changes the interface between engine & client
struct EntityCacheInfo_t
{
	// Cached off because GetClientNetworkable is called a *lot*
	IClientNetworkable *m_pNetworkable;
	unsigned short m_BaseEntitiesIndex;	// Index into m_BaseEntities (or m_BaseEntities.InvalidIndex() if none).
	unsigned short m_bDormant;	// cached dormant state - this is only a bit
};

//-----------------------------------------------------------------------------
// Purpose: Exposes IClientEntity's to engine
//-----------------------------------------------------------------------------
abstract_class IClientEntityList
{
public:
	// Get IClientNetworkable interface for specified entity
	virtual IClientNetworkable*	GetClientNetworkable( int entnum ) = 0;
	virtual IClientNetworkable*	GetClientNetworkableFromHandle( CBaseHandle hEnt ) = 0;
	virtual IClientUnknown*		GetClientUnknownFromHandle( CBaseHandle hEnt ) = 0;

	// NOTE: This function is only a convenience wrapper.
	// It returns GetClientNetworkable( entnum )->GetIClientEntity().
	virtual Entity*		GetClientEntity( int entnum ) = 0;
	virtual Entity*		GetClientEntityFromHandle( CBaseHandle hEnt ) = 0;

	// Returns number of entities currently in use
	virtual int					NumberOfEntities( bool bIncludeNonNetworkable ) = 0;

	// Returns highest index actually used
	virtual int					GetHighestEntityIndex( void ) = 0;

	// Sizes entity list to specified size
	virtual void				SetMaxEntities( int maxents ) = 0;
	virtual int					GetMaxEntities( ) = 0;
	virtual EntityCacheInfo_t	*GetClientNetworkableArray() = 0;


	__forceinline IClientRenderable* GetClientRenderable(int entnum)
	{
		Entity* pEnt = GetClientEntity(entnum);
		if (!pEnt)
			return nullptr;

		return pEnt->GetIClientRenderable();
	}

};

class IClientEntityListener
{
public:
	virtual void OnEntityCreated(Entity* pEntity) {}
	virtual void OnEntityDeleted(Entity* pEntity) {}
};
class CClientEntityList
{
public:
	virtual ~CClientEntityList() {}

	void AddListenerEntity(IClientEntityListener* pListener)
	{
		m_entityListeners.AddToTail(pListener);
	}

	void RemoveListenerEntity(IClientEntityListener* pListener)
	{
		m_entityListeners.FindAndRemove(pListener);
	}

private:
	CUtlVector<IClientEntityListener*, CUtlMemory<IClientEntityListener*>> m_entityListeners;
};
#endif // ICLIENTENTITYLIST_H
