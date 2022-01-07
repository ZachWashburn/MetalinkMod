#if !defined( PREDICTION_H )
#define PREDICTION_H
#ifdef _WIN32
#pragma once
#endif

#include "mathlib/vector.h"
#include "iprediction.h"
#include "Entity.h"

class CMoveData;
class CUserCmd;
class CPDumpPanel;
//-----------------------------------------------------------------------------
// Purpose: Implements prediction in the client .dll
//-----------------------------------------------------------------------------
class CPrediction : public IPrediction
{
// Construction
public:


					CPrediction( void );
	virtual			~CPrediction( void );

	virtual void	Init( void );
	virtual void	Shutdown( void );

// Implement IPrediction
public:

	virtual void	Update
					( 
						int startframe,		// World update ( un-modded ) most recently received
						bool validframe,		// Is frame data valid
						int incoming_acknowledged, // Last command acknowledged to have been run by server (un-modded)
						int outgoing_command	// Last command (most recent) sent to server (un-modded)
					);

	virtual void	OnReceivedUncompressedPacket( void );

	virtual void	PreEntityPacketReceived( int commands_acknowledged, int current_world_update_packet, int server_ticks_elapsed );
	virtual void	PostEntityPacketReceived( void );
	virtual void	PostNetworkDataReceived( int commands_acknowledged );

	virtual bool	InPrediction( void ) const;
	virtual bool	IsFirstTimePredicted( void ) const;
	virtual int		GetLastAcknowledgedCommandNumber( void ) const;

#if defined( KEEP_COMMAND_REPREDICTION_COUNT )
	virtual unsigned int GetRepredictionCount( void ) const;
#endif

#if !defined( NO_ENTITY_PREDICTION )
	virtual int		GetIncomingPacketNumber( void ) const;
#endif

	float			GetIdealPitch( int nSlot ) const 
	{
		throw;
#if 0
		if ( nSlot == -1 )
		{
			Assert( 0 );
			return 0.0f;
		}
		return m_Split[ nSlot ].m_flIdealPitch;
#endif
	}

	// The engine needs to be able to access a few predicted values
	virtual void	GetViewOrigin( Vector& org );
	virtual void	SetViewOrigin( Vector& org );
	virtual void	GetViewAngles( QAngle& ang );
	virtual void	SetViewAngles( QAngle& ang );

	virtual void	GetLocalViewAngles( QAngle& ang );
	virtual void	SetLocalViewAngles( QAngle& ang );

	virtual void	CheckMovingGround( C_BasePlayer *player, double frametime );
	virtual void	RunCommand( C_BasePlayer *player, CUserCmd *ucmd, IMoveHelper *moveHelper );

	float			GetSavedTime() const;

// Internal
protected:
	virtual void	SetupMove( C_BasePlayer *player, CUserCmd *ucmd, IMoveHelper *pHelper, CMoveData *move );
	virtual void	FinishMove( C_BasePlayer *player, CUserCmd *ucmd, CMoveData *move );
	virtual void	SetIdealPitch ( int nSlot, C_BasePlayer *player, const Vector& origin, const QAngle& angles, const Vector& viewheight );

	virtual void	CheckError( int nSlot, C_BasePlayer *player, int commands_acknowledged );

	// Called before and after any movement processing
	void			StartCommand( C_BasePlayer *player, CUserCmd *cmd );
	void			FinishCommand( C_BasePlayer *player );

	// Helpers to call pre and post think for player, and to call think if a think function is set
	void			RunPreThink( C_BasePlayer *player );
	void			RunThink (C_BasePlayer *ent, double frametime );
	void			RunPostThink( C_BasePlayer *player );

private:
	virtual void	_Update
					( 
						int nSlot,
						bool received_new_world_update,
						bool validframe,		// Is frame data valid
						int incoming_acknowledged, // Last command acknowledged to have been run by server (un-modded)
						int outgoing_command	// Last command (most recent) sent to server (un-modded)
					);

	// Actually does the prediction work, returns false if an error occurred
	bool			PerformPrediction( int nSlot, C_BasePlayer *localPlayer, bool received_new_world_update, int incoming_acknowledged, int outgoing_command );

	void			ShiftIntermediateDataForward( int nSlot, int slots_to_remove, int previous_last_slot );
	void			ShiftFirstPredictedIntermediateDataForward( int nSlot, int slots_to_remove );
	void			RestoreEntityToPredictedFrame( int nSlot, int predicted_frame );
	int				ComputeFirstCommandToExecute( int nSlot, bool received_new_world_update, int incoming_acknowledged, int outgoing_command );

	void			DumpEntity( C_BaseEntity *ent, int commands_acknowledged );

	void			ShutdownPredictables( void );
	void			ReinitPredictables( void );

	void			RemoveStalePredictedEntities( int nSlot, int last_command_packet );
	void			RestoreOriginalEntityState( int nSlot );
	void			RunSimulation( int current_command, float curtime, CUserCmd *cmd, C_BasePlayer *localPlayer );
	void			Untouch( int nSlot );
	void			StorePredictionResults( int nSlot, int predicted_frame );
	bool			ShouldDumpEntity( C_BaseEntity *ent );

	void			SmoothViewOnMovingPlatform( C_BasePlayer *pPlayer, Vector& offset );
	void			ResetSimulationTick();
	void			ShowPredictionListEntry( int listRow, int showlist, C_BaseEntity *ent, int &totalsize, int &totalsize_intermediate );
	void			FinishPredictionList( int listRow, int showlist, int totalsize, int totalsize_intermediate );
	void			CheckPredictConvar();


};
 


#endif // PREDICTION_H