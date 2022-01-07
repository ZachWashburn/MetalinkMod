#pragma once
#include "mathlib/mathlib.h"



#include "IClientUnknown.h"
struct model_t;
struct matrix3x4_t;

class IClientModelRenderable;


typedef unsigned short ModelInstanceHandle_t;
typedef unsigned short ClientShadowHandle_t;

enum
{
	CLIENTSHADOW_INVALID_HANDLE = (ClientShadowHandle_t)~0
};

enum ShadowType_t
{
	SHADOWS_NONE = 0,
	SHADOWS_SIMPLE,
	SHADOWS_RENDER_TO_TEXTURE,
	SHADOWS_RENDER_TO_TEXTURE_DYNAMIC,	// the shadow is always changing state
	SHADOWS_RENDER_TO_DEPTH_TEXTURE,
	SHADOWS_RENDER_TO_TEXTURE_DYNAMIC_CUSTOM,	// changing, and entity uses custom rendering code for shadow
};

class IClientRenderable;


//-----------------------------------------------------------------------------
// Handle to an renderable in the client leaf system
//-----------------------------------------------------------------------------
typedef unsigned short ClientRenderHandle_t;

enum
{
	INVALID_CLIENT_RENDER_HANDLE = (ClientRenderHandle_t)0xffff,
};


// This provides a way for entities to know when they've entered or left the PVS.
// Normally, server entities can use NotifyShouldTransmit to get this info, but client-only
// entities can use this. Store a CPVSNotifyInfo in your 
//
// When bInPVS=true, it's being called DURING rendering. It might be after rendering any
// number of views.
//
// If no views had the entity, then it is called with bInPVS=false after rendering.
class IPVSNotify
{
public:
	virtual void OnPVSStatusChanged( bool bInPVS ) = 0;
};


struct RenderableInstance_t
{
	RenderableInstance_t() {}
	RenderableInstance_t(uint8_t Alpha)
	{
		m_nAlpha = Alpha;
	}
	uint8_t m_nAlpha;
};


// client renderable frame buffer usage flags
#define ERENDERFLAGS_NEEDS_POWER_OF_TWO_FB  1				// needs refract texture
#define ERENDERFLAGS_NEEDS_FULL_FB          2				// needs full framebuffer texture
#define ERENDERFLAGS_REFRACT_ONLY_ONCE_PER_FRAME 4 // even if it needs a the refract texture, don't update it >once/ frame


class IClientRenderable
{
public:
	virtual IClientUnknown*	GetIClientUnknown() = 0;
	virtual Vector const&			GetRenderOrigin( void ) = 0;
	virtual QAngle const&			GetRenderAngles( void ) = 0;
	virtual bool					ShouldDraw( void ) = 0;
	virtual int					    GetRenderFlags( void ) = 0; // ERENDERFLAGS_xxx
	virtual void					Unused( void ) const {}
	virtual ClientShadowHandle_t	GetShadowHandle() const = 0;
	virtual ClientRenderHandle_t&	RenderHandle() = 0;
	virtual const model_t*			GetModel( ) const = 0;
	virtual int						DrawModel( int flags, const RenderableInstance_t &instance ) = 0;
	virtual int		GetBody() = 0;
	virtual void	GetColorModulation( float* color ) = 0;
	virtual bool	LODTest() = 0;
	virtual bool	SetupBones( matrix3x4_t *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime ) = 0;
	virtual void	SetupWeights( const matrix3x4_t *pBoneToWorld, int nFlexWeightCount, float *pFlexWeights, float *pFlexDelayedWeights ) = 0;
	virtual void	DoAnimationEvents( void ) = 0;
	virtual IPVSNotify* GetPVSNotifyInterface() = 0;
	virtual void	GetRenderBounds( Vector& mins, Vector& maxs ) = 0;
	virtual void	GetRenderBoundsWorldspace( Vector& mins, Vector& maxs ) = 0;
	virtual void	GetShadowRenderBounds( Vector &mins, Vector &maxs, ShadowType_t shadowType ) = 0;
	virtual bool	ShouldReceiveProjectedTextures( int flags ) = 0;
	virtual bool	GetShadowCastDistance( float *pDist, ShadowType_t shadowType ) const = 0;
	virtual bool	GetShadowCastDirection( Vector *pDirection, ShadowType_t shadowType ) const = 0;
	virtual bool	IsShadowDirty( ) = 0;
	virtual void	MarkShadowDirty( bool bDirty ) = 0;
	virtual IClientRenderable *GetShadowParent() = 0;
	virtual IClientRenderable *FirstShadowChild() = 0;
	virtual IClientRenderable *NextShadowPeer() = 0;
	virtual ShadowType_t ShadowCastType() = 0;
	virtual void Unused2() {}
	// Dont Trust Anything Past Here!!!
#if 0
	virtual void CreateModelInstance() = 0;
	virtual ModelInstanceHandle_t GetModelInstance() = 0;
	virtual const matrix3x4_t &RenderableToWorldTransform() = 0;
	virtual int LookupAttachment( const char *pAttachmentName ) = 0;
	virtual	bool GetAttachment( int number, Vector &origin, QAngle &angles ) = 0;
	virtual bool GetAttachment( int number, matrix3x4_t &matrix ) = 0;
	virtual bool ComputeLightingOrigin( int nAttachmentIndex, Vector modelLightingCenter, const matrix3x4_t &matrix, Vector &transformedLightingCenter ) = 0;
	virtual float *GetRenderClipPlane( void ) = 0;
	virtual int		GetSkin() = 0;
	virtual void	OnThreadedDrawSetup() = 0;
	virtual bool	UsesFlexDelayedWeights() = 0;
	virtual void	RecordToolMessage() = 0;
	virtual bool	ShouldDrawForSplitScreenUser( int nSlot ) = 0;
	virtual uint8_t	OverrideAlphaModulation( uint8_t nAlpha ) = 0;
	virtual uint8_t	OverrideShadowAlphaModulation( uint8_t nAlpha ) = 0;
#else
	SUBFUNC();
	SUBFUNC(); 
	SUBFUNC(); 
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC(); // 31
#endif
	virtual IClientModelRenderable* GetClientModelRenderable() = 0; // 32
};


//-----------------------------------------------------------------------------
// Purpose: All client renderables supporting the fast-path mdl
// rendering algorithm must inherit from this interface
//-----------------------------------------------------------------------------
enum RenderableLightingModel_t
{
	LIGHTING_MODEL_NONE = -1,
	LIGHTING_MODEL_STANDARD = 0,
	LIGHTING_MODEL_STATIC_PROP,
	LIGHTING_MODEL_PHYSICS_PROP,

	LIGHTING_MODEL_COUNT,
};

enum ModelDataCategory_t
{
	MODEL_DATA_LIGHTING_MODEL,	// data type returned is a RenderableLightingModel_t
	MODEL_DATA_STENCIL,			// data type returned is a ShaderStencilState_t

	MODEL_DATA_CATEGORY_COUNT,
};


class IClientModelRenderable
{
public:
	virtual bool GetRenderData( void *pData, ModelDataCategory_t nCategory ) = 0;
};