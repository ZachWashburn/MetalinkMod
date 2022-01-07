/* Only use this shit for sendnetmessage. Else use protobufs!!!!!*/
#pragma once
#include "../BaseIncludes.h"
#include "INetworkChannel.h"
#include "tier1/bitbuf.h"
class SendTable;
class KeyValue;
class INetMessageHandler;
class IServerMessageHandler;
class IClientMessageHandler;
class INetChannel;


class NET_Tick;
class NET_StringCmd;
class NET_SetConVar;
class NET_SignonState;

class CLC_ClientInfo;
class CLC_Move;
class CLC_VoiceData;
class CLC_BaselineAck;
class CLC_ListenEvents;
class CLC_RespondCvarValue;
class CLC_FileCRCCheck;
class CLC_FileMD5Check;
class CLC_SaveReplay;
class CLC_CmdKeyValues;

class bf_read;
class bf_write;



#define DECLARE_BASE_MESSAGE( msgtype )						\
	public:													\
		bool			ReadFromBuffer( bf_read &buffer );	\
		bool			WriteToBuffer( bf_write &buffer );	\
		const char		*ToString() const;					\
		int				GetType() const { return msgtype; } \
		const char		*GetName() const { return #msgtype;}\

#define DECLARE_NET_MESSAGE( name )			\
	DECLARE_BASE_MESSAGE( NetMessageProto::net_##name );		\
	INetMessageHandler *m_pMessageHandler;	\
	bool Process() { return m_pMessageHandler->Process##name( this ); }\

#define DECLARE_SVC_MESSAGE( name )		\
	DECLARE_BASE_MESSAGE( NetMessageProto::svc_##name );	\
	IServerMessageHandler *m_pMessageHandler;\
	bool Process() { return m_pMessageHandler->Process##name( this ); }\

#define DECLARE_CLC_MESSAGE( name )		\
	DECLARE_BASE_MESSAGE( NetMessageProto::clc_##name );	\
	IClientMessageHandler *m_pMessageHandler;\
	bool Process() { return m_pMessageHandler->Process##name( this ); }\

#define DECLARE_MM_MESSAGE( name )		\
	DECLARE_BASE_MESSAGE( mm_##name );	\
	IMatchmakingMessageHandler *m_pMessageHandler;\
	bool Process() { return m_pMessageHandler->Process##name( this ); }\

#define PROCESS_NET_MESSAGE( name )	\
	virtual bool Process##name( NET_##name *msg )

#define PROCESS_SVC_MESSAGE( name )	\
	virtual bool Process##name( SVC_##name *msg )

#define PROCESS_CLC_MESSAGE( name )	\
	virtual bool Process##name( CLC_##name *msg )

#define PROCESS_MM_MESSAGE( name )	\
	virtual bool Process##name( MM_##name *msg )


#if 0 // Defined in netmessages protobufs
enum NET_Messages
{
	net_NOP = 0,
	net_Disconnect = 1,				// disconnect, last message in connection
	net_File = 2,					// file transmission message request/deny
	net_LastControlMessage = 2,
	net_SplitScreenUser = 3,		// Changes split screen user, client and server must both provide handler
	net_Tick = 4, 					// s->c world tick, c->s ack world tick
	net_StringCmd = 5,				// a string command
	net_SetConVar = 6,				// sends one/multiple convar/userinfo settings
	net_SignonState = 7,			// signals or acks current signon state
	net_PlayerAvatarData = 100,
};


enum CLC_Messages
{
	clc_ClientInfo = 8,				// client info (table CRC etc)    
	clc_Move = 9,				// [CUserCmd]     
	clc_VoiceData = 10,				// Voicestream data from a client    
	clc_BaselineAck = 11,				// client acknowledges a new baseline seqnr   
	clc_ListenEvents = 12,				// client acknowledges a new baseline seqnr   
	clc_RespondCvarValue = 13,			// client is responding to a svc_GetCvarValue message.  
	clc_FileCRCCheck = 14,				// client is sending a file's CRC to the server to be verified. 
	clc_LoadingProgress = 15,				// client loading progress  
	clc_SplitPlayerConnect = 16,
	clc_ClientMessage = 17,
	clc_CmdKeyValues = 18,
};


enum SVC_Messages
{
	svc_ServerInfo = 8,		// first message from server about game; map etc
	svc_SendTable = 9,		// sends a sendtable description for a game class
	svc_ClassInfo = 10,		// Info about classes (first byte is a CLASSINFO_ define).							
	svc_SetPause = 11,		// tells client if server paused or unpaused
	svc_CreateStringTable = 12,		// inits shared string tables
	svc_UpdateStringTable = 13,		// updates a string table
	svc_VoiceInit = 14,		// inits used voice codecs & quality
	svc_VoiceData = 15,		// Voicestream data from the server
	svc_Print = 16,	// print text to console
	svc_Sounds = 17,		// starts playing sound
	svc_SetView = 18,		// sets entity as point of view
	svc_FixAngle = 19,		// sets/corrects players viewangle
	svc_CrosshairAngle = 20,		// adjusts crosshair in auto aim mode to lock on traget
	svc_BSPDecal = 21,		// add a static decal to the world BSP
	svc_SplitScreen = 22,		// split screen style message
	svc_UserMessage = 23,		// a game specific message 
	svc_EntityMessage = 24,		// a message for an entity
	svc_GameEvent = 25,		// global game event fired
	svc_PacketEntities = 26,		// non-delta compressed entities
	svc_TempEntities = 27,		// non-reliable event object
	svc_Prefetch = 28,		// only sound indices for now
	svc_Menu = 29,		// display a menu from a plugin
	svc_GameEventList = 30,		// list of known games events and fields
	svc_GetCvarValue = 31,		// Server wants to know the value of a cvar on the client	
	svc_PaintmapData = 33,
	svc_CmdKeyValues = 34,		// Server submits KeyValues command for the client
};
#endif





class INetMessage 
{
public:
	virtual void		 SetNetChannel(INetChannel* pNetChannel) = 0;
	virtual void		 SetReliable(bool bState) = 0;
	virtual bool		 Process() = 0;
	virtual bool		 ReadFromBuffer(bf_read* pBuffer) = 0;
	virtual bool		 WriteToBuffer(bf_read* pBuffer) = 0;
	virtual bool		 IsReliable() = 0;
	virtual int			 GetType() = 0;
	virtual int			 GetGroup() = 0;
	virtual const char*  GetName() = 0;
	virtual INetChannel* GetNetworkChannel() = 0;
	virtual const char* ToString() = 0;
};


class CNetMessage : INetMessage
{
public:
	CNetMessage() {
		m_bReliable = true;
		m_pNetChannel = 0;
	}
	virtual ~CNetMessage() {};
	virtual int		getGroup() const { return net_NOP; }
	INetChannel* getNetChannel() const { return m_pNetChannel; }
	virtual void	SetReliable(bool state) { m_bReliable = state; };
	virtual bool	IsReliable() const { return m_bReliable; };
	virtual void    SetNetChannel(INetChannel* netchan) { m_pNetChannel = netchan; }
	virtual bool	Process() { return false; };	// no handler set
	bool				m_bReliable;	// true if message should be send reliable
	INetChannel* m_pNetChannel;	// netchannel this message is from/for
};


class INetMessageBinder
{
public:
	virtual	~INetMessageBinder() {};
	virtual int	GetType(void) const = 0; // returns module specific header tag eg svc_serverinfo
	virtual void SetNetChannel(INetChannel* netchan) = 0; // netchannel this message is from/for
	virtual INetMessage* CreateFromBuffer(bf_read& buffer) = 0;
	virtual bool Process(const INetMessage& src) = 0;
};


class INetMessageHandler
{
public:
	virtual ~INetMessageHandler(void) {};

	PROCESS_NET_MESSAGE(Tick) = 0;
	PROCESS_NET_MESSAGE(StringCmd) = 0;
	PROCESS_NET_MESSAGE(SetConVar) = 0;
	PROCESS_NET_MESSAGE(SignonState) = 0;
};


class IClientMessageHandler : public INetMessageHandler
{
public:
	virtual ~IClientMessageHandler(void) {};

	PROCESS_CLC_MESSAGE(ClientInfo) = 0;
	PROCESS_CLC_MESSAGE(Move) = 0;
	PROCESS_CLC_MESSAGE(VoiceData) = 0;
	PROCESS_CLC_MESSAGE(BaselineAck) = 0;
	PROCESS_CLC_MESSAGE(ListenEvents) = 0;
	PROCESS_CLC_MESSAGE(RespondCvarValue) = 0;
	PROCESS_CLC_MESSAGE(FileCRCCheck) = 0;
	PROCESS_CLC_MESSAGE(FileMD5Check) = 0;
#if defined( REPLAY_ENABLED )
	PROCESS_CLC_MESSAGE(SaveReplay) = 0;
#endif
	PROCESS_CLC_MESSAGE(CmdKeyValues) = 0;
};








class NET_SignonState : public CNetMessage
{
	//DECLARE_NET_MESSAGE(Tick);
public:
	uint32_t signon_state;
	uint32_t spawn_count;
	uint32_t num_server_players;
	char players_networkids[1024];
	char map_name[1024];

};

class NET_StringCmd : public CNetMessage
{
	//DECLARE_NET_MESSAGE(StringCmd);

	int	getGroup() const { return net_StringCmd; }

	NET_StringCmd() { m_szCommand = NULL; };
	NET_StringCmd(const char* cmd) { m_szCommand = cmd; };

public:
	const char* m_szCommand;	// execute this command

private:
	char		m_szCommandBuffer[1024];	// buffer for received messages

};

class SVC_PacketEntities : public CNetMessage
{
	//DECLARE_SVC_MESSAGE(PacketEntities);

	 //int	GetGroup() const { return INetChannelInfo::ENTITIES; }

public:

	int			m_nMaxEntries;
	int			m_nUpdatedEntries;
	bool		m_bIsDelta;
	bool		m_bUpdateBaseline;
	int			m_nBaseline;
	int			m_nDeltaFrom;
	int			m_nLength;
	bf_read		m_DataIn;
	bf_write	m_DataOut;
};

class CLC_VoiceData : public CNetMessage
{
	//DECLARE_CLC_MESSAGE(VoiceData);

	int	getGroup() const { return clc_VoiceData; }

	CLC_VoiceData() { m_bReliable = false; };

public:
	int				m_nLength;
	bf_read			m_DataIn;
	bf_write		m_DataOut;
	uint64			m_xuid;
};


class CLC_RespondCvarValue : public CNetMessage
{
public:
	int32_t cookie;
	int32_t StatusCode;
	const char* Name;
	const char* Value;
};

class CLC_BaselineAck : public CNetMessage
{
	//DECLARE_CLC_MESSAGE(BaselineAck);

	CLC_BaselineAck() {};
	CLC_BaselineAck(int tick, int baseline)
	{
		m_nBaselineTick = tick;
		m_nBaselineNr = baseline;
	}

	int GetGroup() const
	{
		return 0;//INetChannelInfo::ENTITIES;
	}

public:
	int m_nBaselineTick; // sequence number of baseline
	int m_nBaselineNr;   // 0 or 1
};


class CLC_CmdKeyValues : public CNetMessage
{
	//DECLARE_CLC_MESSAGE(CmdKeyValues);

	int GetGroup() const
	{
		return 0;// net_Gen;
	};
	CLC_CmdKeyValues()
	{
		m_bReliable = false;
	};

public:
};


class CLC_ListenEvents : public CNetMessage {
public:
	int32_t event_mask;
};


#if 0
class CLC_FileCRCCheck : public CNetMessage
{
public:
	DECLARE_CLC_MESSAGE(FileCRCCheck);

	char		m_szPathID[MAX_PATH];
	char		m_szFilename[MAX_PATH];
	CRC32_t		m_CRC;
};
#endif

class CLC_Move : public CNetMessage
{
public:
	uint32_t num_backup_commands;
	uint32_t num_new_commands;
	bf_write data;
};