#pragma once 
#include "../BaseIncludes.h"
#define PACKET_FLAG_RELIABLE			(1<<0)	// packet contains subchannel stream data
#define PACKET_FLAG_COMPRESSED			(1<<1)	// packet is compressed
#define PACKET_FLAG_ENCRYPTED			(1<<2)  // packet is encrypted
#define PACKET_FLAG_SPLIT				(1<<3)  // packet is split
#define PACKET_FLAG_CHOKED				(1<<4)  // packet was choked by sender
// How fast to converge flow estimates
#define FLOW_AVG ( 3.0 / 4.0 )
 // Don't compute more often than this
#define FLOW_INTERVAL 0.25
#define NET_FRAMES_BACKUP	64		// must be power of 2
#define NET_FRAMES_MASK		(NET_FRAMES_BACKUP-1)
#define MAX_SUBCHANNELS		8		// we have 8 alternative send&wait bits
#define SUBCHANNEL_FREE		0	// subchannel is free to use
#define SUBCHANNEL_TOSEND	1	// subchannel has data, but not send yet
#define SUBCHANNEL_WAITING	2   // sbuchannel sent data, waiting for ACK
#define SUBCHANNEL_DIRTY	3	// subchannel is marked as dirty during changelevel
#define MAX_RATE		(1024*1024)				
#define MIN_RATE		1000
#define DEFAULT_RATE	10000
#define SIGNON_TIME_OUT				300.0f  // signon disconnect timeout
#define FRAGMENT_BITS		8
#define FRAGMENT_SIZE		(1<<FRAGMENT_BITS)
#define MAX_FILE_SIZE_BITS	26
#define MAX_FILE_SIZE		((1<<MAX_FILE_SIZE_BITS)-1)	// maximum transferable size is	64MB
// 0 == regular, 1 == file stream
#define MAX_STREAMS			2    

#define	FRAG_NORMAL_STREAM	0
#define FRAG_FILE_STREAM	1

#define TCP_CONNECT_TIMEOUT		4.0f
#define	PORT_ANY				-1
#define PORT_TRY_MAX			10
#define TCP_MAX_ACCEPTS			8

#define LOOPBACK_SOCKETS	2

#define STREAM_CMD_NONE		0	// waiting for next blob
#define STREAM_CMD_AUTH		1	// first command, send back challengenr
#define STREAM_CMD_DATA		2	// receiving a data blob
#define STREAM_CMD_FILE		3	// receiving a file blob
#define STREAM_CMD_ACKN		4	// acknowledged a recveived blob

#define BYTES2FRAGMENTS(i) ((i+FRAGMENT_SIZE-1)/FRAGMENT_SIZE)
// NETWORKING INFO

// This is the packet payload without any header bytes (which are attached for actual sending)
#define	NET_MAX_PAYLOAD			96000	// largest message we can send in bytes
#define NET_MAX_PALYLOAD_BITS	17		// 2^NET_MAX_PALYLOAD_BITS > NET_MAX_PAYLOAD
// This is just the client_t->netchan.datagram buffer size (shouldn't ever need to be huge)
#define NET_MAX_DATAGRAM_PAYLOAD 4000	// = maximum unreliable playload size

// UDP has 28 byte headers
#define UDP_HEADER_SIZE				(20+8)	// IP = 20, UDP = 8


#define MAX_ROUTABLE_PAYLOAD		1260	// Matches x360 size

#if (MAX_ROUTABLE_PAYLOAD & 3) != 0
#error Bit buffers must be a multiple of 4 bytes
#endif

#define MIN_ROUTABLE_PAYLOAD		16		// minimum playload size

#define NETMSG_TYPE_BITS	5	// must be 2^NETMSG_TYPE_BITS > SVC_LASTMSG

// This is the payload plus any header info (excluding UDP header)

#define HEADER_BYTES	9	// 2*4 bytes seqnr, 1 byte flags

// Pad this to next higher 16 byte boundary
// This is the largest packet that can come in/out over the wire, before processing the header
//  bytes will be stripped by the networking channel layer
#define	NET_MAX_MESSAGE	PAD_NUMBER( ( NET_MAX_PAYLOAD + HEADER_BYTES ), 16 )


#define NET_COMPRESSION_STACKBUF_SIZE 4096


#define FLOW_OUTGOING 0
#define FLOW_INCOMING 1

#define MAX_ROUTABLE_PAYLOAD		1260
#define MIN_USER_MAXROUTABLE_SIZE	576  // ( X.25 Networks )
#define MAX_USER_MAXROUTABLE_SIZE	MAX_ROUTABLE_PAYLOAD
#define NET_MAX_MESSAGE 523956


#define MAX_SPLIT_SIZE	(MAX_USER_MAXROUTABLE_SIZE - sizeof( SPLITPACKET ))
#define MIN_SPLIT_SIZE	(MIN_USER_MAXROUTABLE_SIZE - sizeof( SPLITPACKET ))

#define MAX_SPLITPACKET_SPLITS ( NET_MAX_MESSAGE / MIN_SPLIT_SIZE )
#define NET_HEADER_FLAG_SPLITPACKET				-2
#define NET_HEADER_FLAG_COMPRESSEDPACKET		-3
#define CONNECTIONLESS_HEADER			0xFFFFFFFF	// all OOB packet start with this sequence

#define PORT_REPLAY			27040	// Default replay port


// This is used, unless overridden in the registry
#define VALVE_MASTER_ADDRESS "207.173.177.10:27011"

#define PORT_RCON			27015	// Default RCON port, TCP
#define	PORT_MASTER			27011	// Default master port, UDP
#define PORT_CLIENT			27005	// Default client port, UDP/TCP
#define PORT_SERVER			27015	// Default server port, UDP/TCP
#define PORT_HLTV			27020	// Default hltv port
#define PORT_HLTV1			27021	// Default hltv[instance 1] port

#define PORT_X360_RESERVED_FIRST	27026	// X360 reserved port first
#define PORT_X360_RESERVED_LAST		27034	// X360 reserved port last
#ifdef ENABLE_RPT
#define PORT_RPT			27035	// default RPT (remote perf testing) port, TCP
#define PORT_RPT_LISTEN		27036	// RPT connection listener (remote perf testing) port, TCP
#endif // ENABLE_RPT
#define PORT_REPLAY			27040	// Default replay port


#define PAD_NUMBER(number, boundary) \
	( ((number) + ((boundary)-1)) / (boundary) ) * (boundary)

#define MAX_FLOWS 2
#define MAX_STREAMS 2
#define MAX_SUBCHANNELS		8		// we have 8 alternative send&wait bits
#define SUBCHANNEL_FREE		0	// subchannel is free to use
#define SUBCHANNEL_TOSEND	1	// subchannel has data, but not send yet
#define SUBCHANNEL_WAITING	2   // sbuchannel sent data, waiting for ACK
#define SUBCHANNEL_DIRTY	3	// subchannel is marked as dirty during changelevel

#define FRAGMENT_BITS		8
#define FRAGMENT_SIZE		(1<<FRAGMENT_BITS)
#define BYTES2FRAGMENTS(i) ((i+FRAGMENT_SIZE-1)/FRAGMENT_SIZE)
#define NET_MAX_PAYLOAD_BITS 19
#define MAX_FILE_SIZE_BITS 26
#define MAX_FILE_SIZE		((1<<MAX_FILE_SIZE_BITS)-1)	// maximum transferable size is	64MB

#define	FRAG_NORMAL_STREAM	0
#define FRAG_FILE_STREAM	1



#define PORT_RCON			27015	// Default RCON port, TCP
#define	PORT_MASTER			27011	// Default master port, UDP
#define PORT_CLIENT			27005	// Default client port, UDP/TCP
#define PORT_SERVER			27015	// Default server port, UDP/TCP
#define PORT_HLTV			27020	// Default hltv port
#define PORT_HLTV1			27021	// Default hltv[instance 1] port

#define PORT_X360_RESERVED_FIRST	27026	// X360 reserved port first
#define PORT_X360_RESERVED_LAST		27034	// X360 reserved port last
#ifdef ENABLE_RPT
#define PORT_RPT			27035	// default RPT (remote perf testing) port, TCP
#define PORT_RPT_LISTEN		27036	// RPT connection listener (remote perf testing) port, TCP
#endif // ENABLE_RPT
#define PORT_REPLAY			27040	// Default replay port

// out of band message id bytes

// M = master, S = server, C = client, A = any
// the second character will always be \n if the message isn't a single
// byte long (?? not true anymore?)

#define ONNETLANCONNECTIONLESS_HEADER 0
// Requesting for full server list from Server Master
#define	A2M_GET_SERVERS			'c'	// no params

// Master response with full server list
#define	M2A_SERVERS				'd'	// + 6 byte IP/Port list.

// Request for full server list from Server Master done in batches
#define A2M_GET_SERVERS_BATCH	'e' // + in532 uniqueID ( -1 for first batch )

// Master response with server list for channel
#define M2A_SERVER_BATCH		'f' // + int32 next uniqueID( -1 for last batch ) + 6 byte IP/Port list.

// Request for MOTD from Server Master  (Message of the Day)
#define	A2M_GET_MOTD			'g'	// no params

// MOTD response Server Master
#define	M2A_MOTD				'h'	// + string 

// Generic Ping Request
#define	A2A_PING				'i'	// respond with an A2A_ACK

// Generic Ack
#define	A2A_ACK					'j'	// general acknowledgement without info

#define C2S_CONNECT				'k'	// client requests to connect

// Print to client console.
#define	A2A_PRINT				'l'	// print a message on client

// info request
#define S2A_INFO_DETAILED		'm'	// New Query protocol, returns dedicated or not, + other performance info.

#define A2S_RESERVE				'n' // reserves this server for specific players for a short period of time.  Fails if not empty.

#define S2A_RESERVE_RESPONSE	'p' // server response to reservation request

// Another user is requesting a challenge value from this machine
// NOTE: this is currently duplicated in SteamClient.dll but for a different purpose,
// so these can safely diverge anytime. SteamClient will be using a different protocol
// to update the master servers anyway.
#define A2S_GETCHALLENGE		'q'	// Request challenge # from another machine

#define A2S_RCON				'r'	// client rcon command

#define A2A_CUSTOM				't'	// a custom command, follow by a string for 3rd party tools


// A user is requesting the list of master servers, auth servers, and titan dir servers from the Client Master server
#define A2M_GETMASTERSERVERS	'v' // + byte (type of request, TYPE_CLIENT_MASTER or TYPE_SERVER_MASTER)

// Master server list response
#define M2A_MASTERSERVERS		'w'	// + byte type + 6 byte IP/Port List

#define A2M_GETACTIVEMODS		'x' // + string Request to master to provide mod statistics ( current usage ).  "1" for first mod.

#define M2A_ACTIVEMODS			'y' // response:  modname\r\nusers\r\nservers

#define M2M_MSG					'z' // Master peering message

// SERVER TO CLIENT/ANY

// Client connection is initiated by requesting a challenge value
//  the server sends this value back
#define S2C_CHALLENGE			'A' // + challenge value

// Server notification to client to commence signon process using challenge value.
#define	S2C_CONNECTION			'B' // no params

// Response to server info requests

// Request for detailed server/rule information.
#define S2A_INFO_GOLDSRC		'm' // Reserved for use by goldsrc servers

#define S2M_GETFILE				'J'	// request module from master
#define M2S_SENDFILE			'K'	// send module to server

#define S2C_REDIRECT			'L'	// + IP x.x.x.x:port, redirect client to other server/proxy 

#define	C2M_CHECKMD5			'M'	// player client asks secure master if Module MD5 is valid
#define M2C_ISVALIDMD5			'N'	// secure servers answer to C2M_CHECKMD5

// MASTER TO SERVER
#define M2A_ACTIVEMODS3			'P' // response:  keyvalues struct of mods
#define A2M_GETACTIVEMODS3		'Q' // get a list of mods and the stats about them

#define S2A_LOGSTRING			'R'	// send a log string
#define S2A_LOGKEY				'S'	// send a log event as key value
#define S2A_LOGSTRING2			'S'	// send a log string including a secret value << this clashes with S2A_LOGKEY that nothing seems to use in CS:GO and is followed by secret value so should be compatible for server ops with their existing tools

#define A2S_SERVERQUERY_GETCHALLENGE		'W'	// Request challenge # from another machine

#define A2S_KEY_STRING		"Source Engine Query" // required postfix to a A2S_INFO query

#define A2M_GET_SERVERS_BATCH2	'1' // New style server query

#define A2M_GETACTIVEMODS2		'2' // New style mod info query

#define C2S_AUTHREQUEST1        '3' // 
#define S2C_AUTHCHALLENGE1      '4' //
#define C2S_AUTHCHALLENGE2      '5' //
#define S2C_AUTHCOMPLETE        '6'
#define C2S_AUTHCONNECT         '7'  // Unused, signals that the client has
									 // authenticated the server

#define C2S_VALIDATE_SESSION    '8'
// #define UNUSED_A2S_LANSEARCH			'C'	 // LAN game details searches
// #define UNUSED_S2A_LANSEARCHREPLY		'F'	 // LAN game details reply

#define S2C_CONNREJECT			'9'  // Special protocol for rejected connections.

#define MAX_OOB_KEYVALUES		600		// max size in bytes for keyvalues included in an OOB msg
#define MAKE_4BYTES( a, b, c, d ) ( ( ((unsigned char)(d)) << 24 ) | ( ((unsigned char)(c)) << 16 ) | ( ((unsigned char)(b)) << 8 ) | ( ((unsigned char)(a)) << 0 ) )

#define A2A_KV_CMD				'?'	// generic KeyValues command [1 byte: version] [version dependent data...]
#define A2A_KV_VERSION			1	// version of generic KeyValues command
									// [4 bytes: header] [4 bytes: replyid] [4 bytes: challenge] [4 bytes: extra] [4 bytes: numbytes] [numbytes: serialized KV]

// These can be owned by Steam after we get rid of this legacy code.
#define S2A_INFO_SRC			'I'	// + Address, hostname, map, gamedir, gamedescription, active players, maxplayers, protocol
#define	S2M_HEARTBEAT			'a'	// + challeange + sequence + active + #channels + channels
#define S2M_HEARTBEAT2			'0' // New style heartbeat
#define	S2M_SHUTDOWN			'b' // no params
#define M2A_CHALLENGE			's'	// + challenge value 
#define M2S_REQUESTRESTART		'O' // HLMaster rejected a server's connection because the server needs to be updated
#define A2S_RULES				'V'	// request rules list from server
#define S2A_RULES				'E' // + number of rules + string key and string value pairs
#define A2S_INFO				'T' // server info request - this must match the Goldsrc engine
#define S2A_PLAYER				'D' // + Playernum, name, frags, /*deaths*/, time on server
#define A2S_PLAYER				'U'	// request player list

#define A2S_PING2				'Y' // new-style minimalist ping request
#define S2A_PING2REPLY			'Z' // new-style minimalist ping reply


// temp hack until we kill the legacy interface
// The new S2A_INFO_SRC packet has a byte at the end that has these bits in it, telling 
// which data follows.
#define S2A_EXTRA_DATA_HAS_GAME_PORT				0x80		// Next 2 bytes include the game port.
#define S2A_EXTRA_DATA_HAS_SPECTATOR_DATA			0x40		// Next 2 bytes include the spectator port, then the spectator server name.
#define S2A_EXTRA_DATA_HAS_GAMETAG_DATA				0x20		// Next bytes are the game tag string
#define S2A_EXTRA_DATA_HAS_STEAMID					0x10		// Next 8 bytes are the steamID
#define S2A_EXTRA_DATA_GAMEID						0x01		// Next 8 bytes are the gameID of the server

#define A2S_RESERVE_CHECK			'!' // check if server reservation cookie is same as the one we are holding
#define S2A_RESERVE_CHECK_RESPONSE	'%' // server response to reservation request

#define A2S_PING					'$'
#define S2A_PING_RESPONSE			'^'


#define PROTOCOL_AUTHCERTIFICATE 0x01   // Connection from client is using a WON authenticated certificate
#define PROTOCOL_HASHEDCDKEY     0x02	// Connection from client is using hashed CD key because WON comm. channel was unreachable
#define PROTOCOL_STEAM			 0x03	// Steam certificates
#define PROTOCOL_LASTVALID       0x03    // Last valid protocol

#define STEAM_KEYSIZE				2048  // max size needed to contain a steam authentication key (both server and client)


enum CrossPlayPlatform_t
{
	CROSSPLAYPLATFORM_UNKNOWN = 0,
	CROSSPLAYPLATFORM_PC,
	CROSSPLAYPLATFORM_X360,
	CROSSPLAYPLATFORM_PS3,

	CROSSPLAYPLATFORM_LAST = CROSSPLAYPLATFORM_PS3,
};

// each channel packet has 1 byte of FLAG bits
#define PACKET_FLAG_RELIABLE			(1<<0)	// packet contains subchannel stream data
#define PACKET_FLAG_COMPRESSED			(1<<1)	// packet is compressed
#define PACKET_FLAG_ENCRYPTED			(1<<2)  // packet is encrypted
#define PACKET_FLAG_SPLIT				(1<<3)  // packet is split
#define PACKET_FLAG_CHOKED				(1<<4)  // packet was choked by sender
// How fast to converge flow estimates
#define FLOW_AVG ( 3.0 / 4.0 )
 // Don't compute more often than this
#define FLOW_INTERVAL 0.25


#define NET_FRAMES_BACKUP	64		// must be power of 2
#define NET_FRAMES_MASK		(NET_FRAMES_BACKUP-1)
#define MAX_SUBCHANNELS		8		// we have 8 alternative send&wait bits

#define SUBCHANNEL_FREE		0	// subchannel is free to use
#define SUBCHANNEL_TOSEND	1	// subchannel has data, but not send yet
#define SUBCHANNEL_WAITING	2   // sbuchannel sent data, waiting for ACK
#define SUBCHANNEL_DIRTY	3	// subchannel is marked as dirty during changelevel
#define MAX_RATE		(1024*1024)				
#define MIN_RATE		1000
#define DEFAULT_RATE	10000

#define SIGNON_TIME_OUT				300.0f  // signon disconnect timeout

#define FRAGMENT_BITS		8
#define FRAGMENT_SIZE		(1<<FRAGMENT_BITS)
#define MAX_FILE_SIZE_BITS	26
#define MAX_FILE_SIZE		((1<<MAX_FILE_SIZE_BITS)-1)	// maximum transferable size is	64MB

// 0 == regular, 1 == file stream
#define MAX_STREAMS			2    

#define	FRAG_NORMAL_STREAM	0
#define FRAG_FILE_STREAM	1

#define TCP_CONNECT_TIMEOUT		4.0f
#define	PORT_ANY				-1
#define PORT_TRY_MAX			10
#define TCP_MAX_ACCEPTS			8

#define LOOPBACK_SOCKETS	2

#define STREAM_CMD_NONE		0	// waiting for next blob
#define STREAM_CMD_AUTH		1	// first command, send back challengenr
#define STREAM_CMD_DATA		2	// receiving a data blob
#define STREAM_CMD_FILE		3	// receiving a file blob
#define STREAM_CMD_ACKN		4	// acknowledged a recveived blob

#define BYTES2FRAGMENTS(i) ((i+FRAGMENT_SIZE-1)/FRAGMENT_SIZE)
// NETWORKING INFO

// This is the packet payload without any header bytes (which are attached for actual sending)
#define	NET_MAX_PAYLOAD			96000	// largest message we can send in bytes
#define NET_MAX_PALYLOAD_BITS	17		// 2^NET_MAX_PALYLOAD_BITS > NET_MAX_PAYLOAD
// This is just the client_t->netchan.datagram buffer size (shouldn't ever need to be huge)
#define NET_MAX_DATAGRAM_PAYLOAD 4000	// = maximum unreliable playload size

// UDP has 28 byte headers
#define UDP_HEADER_SIZE				(20+8)	// IP = 20, UDP = 8


#define MAX_ROUTABLE_PAYLOAD		1260	// Matches x360 size

#if (MAX_ROUTABLE_PAYLOAD & 3) != 0
#error Bit buffers must be a multiple of 4 bytes
#endif

#define MIN_ROUTABLE_PAYLOAD		16		// minimum playload size

#define NETMSG_TYPE_BITS	5	// must be 2^NETMSG_TYPE_BITS > SVC_LASTMSG

// This is the payload plus any header info (excluding UDP header)

#define HEADER_BYTES	9	// 2*4 bytes seqnr, 1 byte flags

// Pad this to next higher 16 byte boundary
// This is the largest packet that can come in/out over the wire, before processing the header
//  bytes will be stripped by the networking channel layer
#define	NET_MAX_MESSAGE	PAD_NUMBER( ( NET_MAX_PAYLOAD + HEADER_BYTES ), 16 )


#define NET_COMPRESSION_STACKBUF_SIZE 4096


#define FLOW_OUTGOING 0
#define FLOW_INCOMING 1

#define MAX_ROUTABLE_PAYLOAD		1260
#define MIN_USER_MAXROUTABLE_SIZE	576  // ( X.25 Networks )
#define MAX_USER_MAXROUTABLE_SIZE	MAX_ROUTABLE_PAYLOAD
#define NET_MAX_MESSAGE 523956

#define MAX_SPLIT_SIZE	(MAX_USER_MAXROUTABLE_SIZE - sizeof( SPLITPACKET ))
#define MIN_SPLIT_SIZE	(MIN_USER_MAXROUTABLE_SIZE - sizeof( SPLITPACKET ))

#define MAX_SPLITPACKET_SPLITS ( NET_MAX_MESSAGE / MIN_SPLIT_SIZE )

#define SIGNONSTATE_NONE		0	// no state yet, about to connect
#define SIGNONSTATE_CHALLENGE	1	// client challenging server, all OOB packets
#define SIGNONSTATE_CONNECTED	2	// client is connected to server, netchans ready
#define SIGNONSTATE_NEW			3	// just got serverinfo and string tables
#define SIGNONSTATE_PRESPAWN	4	// received signon buffers
#define SIGNONSTATE_SPAWN		5	// ready to receive entity packets
#define SIGNONSTATE_FULL		6	// we are fully connected, first non-delta packet received
#define SIGNONSTATE_CHANGELEVEL	7	// server is changing level, please wait




#define NET_HEADER_FLAG_SPLITPACKET				-2
#define NET_HEADER_FLAG_COMPRESSEDPACKET		-3