//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
// netadr.h
#ifndef NETADR_H
#define NETADR_H
#ifdef _WIN32
#pragma once
#endif
#undef SetPort
#include "tier0/platform.h"
#include "../BaseIncludes.h"
class bf_read;
class bf_write;

typedef enum
{ 
	NA_NULL = 0,
	NA_LOOPBACK,
	NA_BROADCAST,
	NA_IP,
} netadrtype_t;

typedef struct netadr_s
{
public:
	netadr_s() { SetIP( 0 ); SetPort( 0 ); SetType( NA_IP ); }
	netadr_s( uint unIP, uint16 usPort ) { SetIP( unIP ); SetPort( usPort ); SetType( NA_IP ); }
	netadr_s( const char *pch ) { SetFromString( pch ); }
	void	Clear();	// invalids Address

	void	SetType( netadrtype_t type );
	void	SetPort( unsigned short port );
	bool	SetFromSockadr(const struct sockaddr *s);
	void	SetIP(uint8 b1, uint8 b2, uint8 b3, uint8 b4);
	void	SetIP(uint unIP);									// Sets IP.  unIP is in host order (little-endian)
	void    SetIPAndPort( uint unIP, unsigned short usPort ) { SetIP( unIP ); SetPort( usPort ); }
	bool	SetFromString(const char *pch, bool bUseDNS = false ); // if bUseDNS is true then do a DNS lookup if needed
	
	bool	CompareAdr (const netadr_s &a, bool onlyBase = false) const;
	bool	CompareClassBAdr (const netadr_s &a) const;
	bool	CompareClassCAdr (const netadr_s &a) const;

	netadrtype_t	GetType() const;
	unsigned short	GetPort() const;

	// DON'T CALL THIS
	const char*		ToString( bool onlyBase = false ) const; // returns xxx.xxx.xxx.xxx:ppppp

	void	ToString( char *pchBuffer, uint32 unBufferSize, bool onlyBase = false ) const; // returns xxx.xxx.xxx.xxx:ppppp
	template< size_t maxLenInChars >
	void	ToString_safe( char (&pDest)[maxLenInChars], bool onlyBase = false ) const
	{
		ToString( &pDest[0], maxLenInChars, onlyBase );
	}

	void			ToSockadr(struct sockaddr *s) const;

	// Returns 0xAABBCCDD for AA.BB.CC.DD on all platforms, which is the same format used by SetIP().
	// (So why isn't it just named GetIP()?  Because previously there was a fucntion named GetIP(), and
	// it did NOT return back what you put into SetIP().  So we nuked that guy.)
	unsigned int	GetIPHostByteOrder() const;

	// Returns a number that depends on the platform.  In most cases, this probably should not be used.
	unsigned int	GetIPNetworkByteOrder() const;

	bool	IsLocalhost() const; // true, if this is the localhost IP 
	bool	IsLoopback() const;	// true if engine loopback buffers are used
	bool	IsReservedAdr() const; // true, if this is a private LAN IP
	bool	IsValid() const;	// ip & port != 0
	bool	IsBaseAdrValid() const;	// ip != 0

	void    SetFromSocket( int hSocket );

	bool	Unserialize( bf_read &readBuf );
	bool	Serialize( bf_write &writeBuf );

	bool operator==(const netadr_s &netadr) const {return ( CompareAdr( netadr ) );}
	bool operator!=(const netadr_s &netadr) const {return !( CompareAdr( netadr ) );}
	bool operator<(const netadr_s &netadr) const;

public:	// members are public to avoid to much changes

	netadrtype_t	type;
	unsigned char	ip[4];
	unsigned short	port;
} netadr_t;


/// Helper class to render a netadr_t.  Use this when formatting a net address
/// in a printf.  Don't use adr.ToString()!
class CUtlNetAdrRender
{
public:
	CUtlNetAdrRender( const netadr_t &obj, bool bBaseOnly = false )
	{
		obj.ToString( m_rgchString, sizeof(m_rgchString), bBaseOnly );
	}

	CUtlNetAdrRender( uint32 unIP )
	{
		netadr_t addr( unIP, 0 );
		addr.ToString( m_rgchString, sizeof(m_rgchString), true );
	}

	CUtlNetAdrRender( uint32 unIP, uint16 unPort )
	{
		netadr_t addr( unIP, unPort );
		addr.ToString( m_rgchString, sizeof(m_rgchString), false );
	}

	CUtlNetAdrRender( const struct sockaddr &s )
	{
		netadr_t addr;
		if ( addr.SetFromSockadr( &s ) )
			addr.ToString( m_rgchString, sizeof(m_rgchString), false );
		else
			m_rgchString[0] = '\0';
	}

	const char * String() const
	{ 
		return m_rgchString;
	}

private:

	char m_rgchString[32];
};

enum PeerToPeerAddressType_t
{
	P2P_STEAMID,
};


enum NetworkSystemAddressType_t
{
	NSAT_NETADR,
	NSAT_P2P,
	NSAT_PROXIED_GAMESERVER,	// Client proxied through Steam Datagram Transport
	NSAT_PROXIED_CLIENT,		// Client proxied through Steam Datagram Transport
};

class CPeerToPeerAddress
{
public:
	uint64 m_steamID; //CSteamID
	int m_steamChannel; // SteamID channel (like a port number to disambiguate multiple connections)

	PeerToPeerAddressType_t m_AddrType;

	template <typename T> bool IsType(void) const
	{
		return false;
	}

	template <>
	inline bool IsType<CSteamID>(void) const
	{
		return (m_AddrType == P2P_STEAMID);
	}
};


#pragma pack(1)
struct ns_address
{
	netadr_t m_adr; // ip:port and network type (NULL/IP/BROADCAST/etc).
	CPeerToPeerAddress m_steamID; // SteamID destination 16 bytes
	NetworkSystemAddressType_t m_AddrType; //4 bytes

	template <typename T> bool IsType(void) const
	{
		//defer to subtype
		switch (m_AddrType)
		{
		case NSAT_P2P:
			return m_steamID.IsType<T>();
		};
		return false;
	}

	template <>
	inline bool IsType<netadr_t>(void) const
	{
		return (m_AddrType == NSAT_NETADR);
	}

	template <>
	inline bool IsType<CPeerToPeerAddress>(void) const
	{
		return (m_AddrType == NSAT_P2P);
	}

	bool IsLoopback(void) const
	{
		return (m_AddrType == NSAT_NETADR) && m_adr.IsLoopback();
	}

	bool IsLocalhost(void) const
	{
		return (m_AddrType == NSAT_NETADR) && m_adr.IsLocalhost();
	}

	bool IsBroadcast(void) const
	{
		return (m_AddrType == NSAT_NETADR) && m_adr.GetType() == NA_BROADCAST;
	}

	bool IsReservedAdr(void) const
	{
		return (m_AddrType == NSAT_NETADR) && m_adr.IsReservedAdr();
	}
};
#pragma pack(pop)


#endif // NETADR_H
