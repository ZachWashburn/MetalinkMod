//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#ifndef VPANEL_H
#define VPANEL_H

#ifdef _WIN32
#pragma once
#endif

class VPanel;
class SurfacePlat;
class SurfacePlat;
#include "vgui.h"
#include "tier1/utlvector.h"

#ifdef GetClassName
#undef GetClassName
#endif

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------

namespace vgui
{

class SurfaceBase;
class IClientPanel;
struct SerialPanel_t;

//-----------------------------------------------------------------------------
// Purpose: VGUI private implementation of panel
//-----------------------------------------------------------------------------
class VPanel
{
	//DECLARE_DMXELEMENT_UNPACK_NAMESPACE(vgui);

public:
	VPanel();
	virtual ~VPanel();

	virtual void Init(IClientPanel *attachedClientPanel);

	virtual SurfacePlat *Plat();
	virtual void SetPlat(SurfacePlat *pl);

	virtual HPanel GetHPanel() { throw; } // safe pointer handling
	virtual void SetHPanel(HPanel hPanel) { throw; }

	virtual bool IsPopup();
	virtual void SetPopup(bool state);
	virtual bool IsFullyVisible();

	virtual void SetPos(int x, int y);
	virtual void GetPos(int &x, int &y);
	virtual void SetSize(int wide,int tall);
	virtual void GetSize(int& wide,int& tall);
	virtual void SetMinimumSize(int wide,int tall);
	virtual void GetMinimumSize(int& wide,int& tall);
	virtual void SetZPos(int z);
	virtual int  GetZPos();

	virtual void GetAbsPos(int &x, int &y);
	virtual void GetClipRect(int &x0, int &y0, int &x1, int &y1);
	virtual void SetInset(int left, int top, int right, int bottom);
	virtual void GetInset(int &left, int &top, int &right, int &bottom);

	virtual void Solve();

	virtual void SetVisible(bool state);
	virtual void SetEnabled(bool state);
	virtual bool IsVisible();
	virtual bool IsEnabled();
	virtual void SetParent(VPanel *newParent);
	virtual int GetChildCount();
	virtual VPanel *GetChild(int index);
	virtual CUtlVector< VPanel * > &GetChildren();
	virtual VPanel *GetParent();
	virtual void MoveToFront();
	virtual void MoveToBack();
	virtual bool HasParent(VPanel *potentialParent);

	// gets names of the object (for debugging purposes)
	virtual const char *GetName();
	virtual const char *GetClassName();

	//virtual HScheme GetScheme();
	SUBFUNC();
	// handles a message
	SUBFUNC();
	//virtual void SendMessage(KeyValues *params, VPANEL ifromPanel);

	// wrapper to get Client panel interface
	virtual IClientPanel *Client() {  }

	// input interest
	virtual void SetKeyBoardInputEnabled(bool state);
	virtual void SetMouseInputEnabled(bool state);
	virtual bool IsKeyBoardInputEnabled();
	virtual bool IsMouseInputEnabled();

	virtual bool IsTopmostPopup() const;
	virtual void SetTopmostPopup( bool bEnable );

	virtual void SetMessageContextId( int nContextId );
	virtual int GetMessageContextId();

	virtual void OnUnserialized( /*CDmxElement *pElement*/ );
	// sibling pins
	virtual void SetSiblingPin(VPanel *newSibling, byte iMyCornerToPin = 0, byte iSiblingCornerToPinTo = 0 );

public:
	virtual void GetInternalAbsPos(int &x, int &y);
private:


};

}


#endif // VPANEL_H