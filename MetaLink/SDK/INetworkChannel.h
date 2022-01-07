#include "../../BaseIncludes.h"
#pragma once
#include "NetDefines.h"
#include "tier1/netadr.h"
#include "tier1/bitbuf.h"
#include "tier1/utlmemory.h"
#include "tier1/utlvector.h"
#include <stdint.h>
class INetMessageBinder;
class INetkMessage;
class bf_write;
struct netpacket_s;
class INetChannelHandler;
class INetChannelInfo;
class NetMsgBinder;

#if 0
int8_t  m_StreamReliable[24];
int8_t  m_StreamUnreliable[24];
int8_t  m_UnreliableDataBuffer[12];
int8_t m_StreamVoice[24];
int8_t m_VoiceDataBuffer[12];
#endif
#pragma pack(push, 1)
typedef unsigned int uint;
typedef unsigned char uint8;
#if 1

class INetChannelInfo
{
public:

	enum {
		GENERIC = 0,	// must be first and is default group
		LOCALPLAYER,	// bytes for local player entity update
		OTHERPLAYERS,	// bytes for other players update
		ENTITIES,		// all other entity bytes
		SOUNDS,			// game sounds
		EVENTS,			// event messages
		TEMPENTS,		// temp entities
		USERMESSAGES,	// user messages
		ENTMESSAGES,	// entity messages
		VOICE,			// voice data
		STRINGTABLE,	// a stringtable update
		MOVE,			// client move cmds
		STRINGCMD,		// string command
		SIGNON,			// various signondata
		TOTAL,			// must be last and is not a real group
		PAINTMAP,
	};

	virtual const char* GetName(void) const = 0;	// get channel name
	virtual const char* GetAddress(void) const = 0; // get channel IP address as string
	virtual float		GetTime(void) const = 0;	// current net time
	virtual float		GetTimeConnected(void) const = 0;	// get connection time in seconds
	virtual int			GetBufferSize(void) const = 0;	// netchannel packet history size
	virtual int			GetDataRate(void) const = 0; // send data rate in byte/sec

	virtual bool		IsLoopback(void) const = 0;	// true if loopback channel
	virtual bool		IsTimingOut(void) const = 0;	// true if timing out
	virtual bool		IsPlayback(void) const = 0;	// true if demo playback

	virtual float		GetLatency(int flow) const = 0;	 // current latency (RTT), more accurate but jittering
	virtual float		GetAvgLatency(int flow) const = 0; // average packet latency in seconds
	virtual float		GetAvgLoss(int flow) const = 0;	 // avg packet loss[0..1]
	virtual float		GetAvgChoke(int flow) const = 0;	 // avg packet choke[0..1]
	virtual float		GetAvgData(int flow) const = 0;	 // data flow in bytes/sec
	virtual float		GetAvgPackets(int flow) const = 0; // avg packets/sec
	virtual int			GetTotalData(int flow) const = 0;	 // total flow in/out in bytes
	virtual int			GetTotalPackets(int flow) const = 0;
	virtual int			GetSequenceNr(int flow) const = 0;	// last send seq number
	virtual bool		IsValidPacket(int flow, int frame_number) const = 0; // true if packet was not lost/dropped/chocked/flushed
	virtual float		GetPacketTime(int flow, int frame_number) const = 0; // time when packet was send
	virtual int			GetPacketBytes(int flow, int frame_number, int group) const = 0; // group size of this packet
	virtual bool		GetStreamProgress(int flow, int* received, int* total) const = 0;  // TCP progress if transmitting
	virtual float		GetTimeSinceLastReceived(void) const = 0;	// get time since last recieved packet in seconds
	virtual	float		GetCommandInterpolationAmount(int flow, int frame_number) const = 0;
	virtual void		GetPacketResponseLatency(int flow, int frame_number, int* pnLatencyMsecs, int* pnChoke) const = 0;
	virtual void		GetRemoteFramerate(float* pflFrameTime, float* pflFrameTimeStdDeviation) const = 0;

	virtual float		GetTimeoutSeconds() const = 0;
};
#endif

struct subChannel_s
{
	int				startFraggment[MAX_STREAMS];
	int				numFragments[MAX_STREAMS];
	int				sendSeqNr;
	int				state; // 0 = free, 1 = scheduled to send, 2 = send & waiting, 3 = dirty
	int				index; // index in m_SubChannels[]

	void Free()
	{
		state = SUBCHANNEL_FREE;
		sendSeqNr = -1;
		for (int i = 0; i < MAX_STREAMS; i++)
		{
			numFragments[i] = 0;
			startFraggment[i] = -1;
		}
	}
};

// Client's now store the command they sent to the server and the entire results set of
//  that command. 
typedef struct netframe_s
{
	// Data received from server
	float			time;			// net_time received/send
	int				size;			// total size in bytes
	float			latency;		// raw ping for this packet, not cleaned. set when acknowledged otherwise -1.
	float			avg_latency;	// averaged ping for this packet
	bool			valid;			// false if dropped, lost, flushed
	int				choked;			// number of previously chocked packets
	int				dropped;
	float			m_flInterpolationAmount;
	unsigned short	msggroups[INetChannelInfo::TOTAL];	// received bytes for each message group
} netframe_t;


typedef struct
{
	float		nextcompute;	// Time when we should recompute k/sec data
	float		avgbytespersec;	// average bytes/sec
	float		avgpacketspersec;// average packets/sec
	float		avgloss;		// average packet loss [0..1]
	float		avgchoke;		// average packet choke [0..1]
	float		avglatency;		// average ping, not cleaned
	float		latency;		// current ping, more accurate also more jittering
	int			totalpackets;	// total processed packets
	int			totalbytes;		// total processed bytes
	int			currentindex;		// current frame index
	netframe_t	frames[NET_FRAMES_BACKUP]; // frame history
	netframe_t* currentframe;	// current frame
} netflow_t;





#pragma pack(push, 1)
typedef struct netpacket_s
{
	ns_address addr;
	int source; // +32
	int unknown_int;// char pad2[4];
	double received; // + 40
	char* data; // + 48
	//bf_read message;// 
	char message[36];
	int size; // + 88
	int wiresize; // +92
	int stream; // 96
	struct netpacket_s* pNext; // + 100
} netpacket_t;
#pragma pack(pop)


//inline int dkdk = sizeof(netpacket_s);


#if 0
typedef struct netpacket_s
{
	netadr_t		from;		// sender IP
	int				source;		// received source 
	double			received;	// received time
	unsigned char* data;		// pointer to raw packet data
	bf_read			message;	// easy bitbuf data access
	int				size;		// size in bytes
	int				wiresize;   // size in bytes before decompression
	bool			stream;		// was send as stream
	struct netpacket_s* pNext;	// for internal use, should be NULL in public
} netpacket_t;
#endif


//inline int j = sizeof(bf_read);
//inline int k = sizeof(netpacket_s);

typedef struct dataFragments_s
{

	void* file;			// open file handle
	char			filename[MAX_PATH]; // filename
	//std::byte pad[61];
	char* buffer;			// if NULL it's a file
	unsigned int	bytes;			// size in bytes
	unsigned int	bits;			// size in bits
	unsigned int	transferID;		// only for files
	bool			isCompressed;	// true if data is bzip compressed
	unsigned int	nUncompressedSize; // full size in bytes
	bool			asTCP;			// send as TCP stream
	bool            isReplayDemo;
	int				numFragments;	// number of total fragments
	int				ackedFragments; // number of fragments send & acknowledged
	int				pendingFragments; // number of fragments send, but not acknowledged yet
} dataFragments_t;




// Use this to pick apart the network stream, must be packed
#pragma pack(1)
typedef struct
{
	int		netID;
	int		sequenceNumber;
	int		packetID : 16;
	int		nSplitSize : 16;
} SPLITPACKET;
#pragma pack()


#pragma pack(push, 1)
class INetChannel {
public:
	virtual const char*			 GetName(void) = 0;
	virtual const char*			 GetAddress(void) = 0; // Everything after this im not 100% sure
	virtual float				 GetTime(void) const = 0;	// current net time
	virtual float				 GetTimeConnected(void) const = 0;	// get connection time in seconds
	virtual int					 GetBufferSize(void) const = 0;	// netchannel packet history size
	virtual int					 GetDataRate(void) const = 0; // send data rate in byte/sec
	virtual bool				 IsLoopback(void) const = 0;	// true if loopback channel
	virtual bool				 IsTimingOut(void) const = 0;	// true if timing out
	virtual bool				 IsPlayback(void) const = 0;	// true if demo playback
	virtual float				 GetLatency(int nFlow) = 0; // 9
	virtual float				 GetAvgLatency(int flow) const = 0; // average packet latency in seconds
	virtual float				 GetAvgLoss(int flow) const = 0;	 // avg packet loss[0..1]
	virtual float				 GetAvgChoke(int flow) const = 0;	 // avg packet choke[0..1]
	virtual float				 GetAvgData(int flow) const = 0;	 // data flow in bytes/sec
	virtual float				 GetAvgPackets(int flow) const = 0; // avg packets/sec
	virtual int					 GetTotalData(int flow) const = 0;	 // total flow in/out in bytes
	virtual int					 GetTotalPackets(int flow) const = 0;
	virtual int					 GetSequenceNr(int flow) const = 0;	// last send seq number
	virtual bool				 IsValidPacket(int flow, int frame_number) const = 0; // true if packet was not lost/dropped/chocked/flushed
	virtual float				 GetPacketTime(int flow, int frame_number) const = 0; // time when packet was send
	virtual int					 GetPacketBytes(int flow, int frame_number, int group) const = 0; // group size of this packet
	virtual bool				 GetStreamProgress(int flow, int* received, int* total) const = 0;  // TCP progress if transmitting
	virtual float				 GetTimeSinceLastReceived(void) const = 0;	// get time since last recieved packet in seconds
	virtual	float				 GetCommandInterpolationAmount(int flow, int frame_number) const = 0;
	virtual void				 GetPacketResponseLatency(int flow, int frame_number, int* pnLatencyMsecs, int* pnChoke) const = 0;
	virtual void				 GetRemoteFramerate(float* pflFrameTime, float* pflFrameTimeStdDeviation, float* pflFrameStartTimeStdDeviation) const = 0;
	virtual float				 GetTimeoutSeconds() const = 0; // Everything before this im not 100% sure
	virtual INetMessageBinder*   FindMessageBinder(int nType, int nIndex) = 0; // 27
	virtual bool				 RegisterMessage(INetMessageBinder* pMsgBinder) = 0; // 28
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	virtual void				 ProcessPlayback(void) = 0;
	virtual bool				 ProcessStream(void) = 0;
	virtual void				 ProcessPacket(netpacket_s* pPacket, bool bHasHeader) = 0; // 39
	virtual bool				 SendNetMsg(INetkMessage* pMsg, bool bForceReliable, bool bVoice) = 0; // 40
	virtual bool				 SendData(bf_write& msg, bool bReliable = true) = 0; // 41
	virtual bool				 SendFile(const char* szFilename, unsigned int nTransferID, bool bIsReplayDemoFile) = 0;
	virtual void				 DenyFile(const char* szFilename, unsigned int nTransferID, bool bIsReplayDemoFile) = 0;
	virtual void				 RequestFile_OLD(const char* szFilename, unsigned int nTransferID) = 0;
	virtual void				 SetChoked(void) = 0;
	virtual int					 SendDatagram(bf_write* pData) = 0; // 46
	virtual bool				 Transmit(bool bOnlyReliable) = 0;
	SUBFUNC();                   // virtual const ns_address& GetRemoteAddress(void) const = 0;
	virtual INetChannelHandler*  GetMsgHandler(void) const = 0;
	virtual int				     GetDropNumber(void) const = 0;
	virtual int					 GetSocket(void) const = 0;
	virtual unsigned int		 GetChallengeNr(void) const = 0;
	virtual void				 GetSequenceData(int& nOutSequenceNr, int& nInSequenceNr, int& nOutSequenceNrAck) = 0;
	virtual void				 SetSequenceData(int nOutSequenceNr, int nInSequenceNr, int nOutSequenceNrAck) = 0;
	virtual void				 UpdateMessageStats(int nMsgGroup, int nBits) = 0;
	virtual bool				 CanPacket(void) const = 0;
	SUBFUNC();
	SUBFUNC();
	virtual void				 SetFileTransmissionMode(bool bBackgroundMode) = 0;
	virtual void				 SetCompressionMode(bool bUseCompression) = 0;
	virtual unsigned int		 RequestFile(const char* szFilename, bool bIsReplayDemoFile) = 0;
	SUBFUNC();			//virtual float				 GetTimeSinceLastReceived(void) const = 0;	// If you want to use this functionality, use The Logging Class Instead!!!!
	virtual void				 SetMaxBufferSize(bool bReliable, int nBytes, bool bVoice = false) = 0;
	virtual bool				 IsNull() const = 0;
	virtual int					 GetNumBitsWritten(bool bReliable) = 0;
	virtual void				 SetInterpolationAmount(float flInterpolationAmount) = 0;
	virtual void				 SetRemoteFramerate(float flFrameTime, float flFrameTimeStdDeviation, float flFrameStartTimeStdDeviation) = 0;
	virtual void				 SetMaxRoutablePayloadSize(int nSplitSize) = 0;
	virtual int					 GetMaxRoutablePayloadSize() = 0;
	virtual bool				 SetActiveChannel(INetChannel* pNewChannel) = 0;
	virtual void				 AttachSplitPlayer(int nSplitPlayerSlot, INetChannel* pChannel) = 0;
	virtual void				 DetachSplitPlayer(int nSplitPlayerSlot) = 0;
	virtual bool				 IsRemoteDisconnected() const = 0;
	virtual const unsigned char* GetChannelEncryptionKey(void) = 0;
	virtual bool				 EnqueueVeryLargeAsyncTransfer(INetkMessage& msg) = 0;	// Enqueues a message for a large async transfer


	NetMsgBinder* m_Binder1; //0x0004 
	NetMsgBinder* m_Binder2; //0x0008 
	NetMsgBinder* m_Binder3; //0x000C 
	NetMsgBinder* m_Binder4; //0x0010 
	unsigned char m_bProcessingMessages;
	unsigned char m_bShouldDelete;
	PAD(0x2);
    int m_nOutSequenceNr;
    int m_nInSequenceNr;
    int m_nOutSequenceNrAck;
    int m_nOutReliableState;
    int m_nInReliableState;
    int m_nChokedPackets; 
	bf_write m_StreamReliable;
	CUtlMemory<std::byte> m_ReliableDataBuffer;
	bf_write m_StreamUnreliable;
	CUtlMemory<std::byte> m_UnreliableDataBuffer;
	bf_write m_StreamVoice;
	CUtlMemory<std::byte> m_VoiceDataBuffer;
	int32_t m_nSocket; 
	int32_t m_nMaxReliablePayloadSize; 
	bool m_bWasLastMessageReliable;
	PAD(0x3);
	ns_address m_RemoteAddress; 
	char m_szRemoteAddressName[64]; 
	PAD(0x4);
	float m_flLastReceived; //0x10C
	double connect_time; //0x110
	int32_t m_nRate; 
	PAD(0x4);
	double m_fClearTime;
	CUtlVector<dataFragments_t*>	m_WaitingList[MAX_STREAMS];
	PAD(0x260); 
	subChannel_s					m_SubChannels[MAX_SUBCHANNELS]; 
	PAD(12);
	netflow_t m_DataFlow[MAX_FLOWS];
	int	m_MsgStats[16];	// total bytes for each message group
	int32_t m_nPacketDrop; 
	char m_szName[32]; 
	int32_t m_nChallengeNr; 
	float m_fkTimeout;
	void* m_MessageHandler;
	int8_t m_NetMessages[16];
	PAD(0x4);
	void* m_pDemoRecorder;
	float m_flInterpolationAmount; 
	double m_flRemoteFrameTime;
	float m_flRemoteFrameTimeStdDeviation; 
	int32_t m_nMaxRoutablePayloadSize; 
	int32_t m_nSplitPacketSequence; 
	PAD(0x14); 
};
#pragma pack(pop)



