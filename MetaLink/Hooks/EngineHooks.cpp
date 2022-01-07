#include "EngineHooks.h"
#include "../Memory.h"
#include <intrin.h>
#include "SDK.h"
#include "NetDefines.h"


bool __fastcall hk_IsHLTV(void* pEngine, void*) {
	if (_ReturnAddress() == g_pMemory->GetAddress(__SETUPVELOCITYRET))
		return true;


	if (_ReturnAddress() == g_pMemory->GetAddress(__ACCUMULATELAYERSRET)) {
		return true;
	}

	return oIsHLTV(pEngine);
}


typedef signed int(__cdecl* Calls_SendToImplFunc2)(int* buffer, unsigned char* socket, signed int size, int pVoice);
static int callAmount = 0;
static bool inLoop = false;
unsigned char* compressPacket = NULL;
int CUSTOM_NET_SendLong(void* chan, unsigned char* sendbuf, int sendlen, int nMaxRoutableSize, bool Encrypt/* = false*/, bool /*Decrypt = true*/)
{
	
}

struct DeferredConnection_t
{
	DeferredConnection_t()
	{
		m_bActive = false;
		m_nChallenge = 0;
		m_nAuthprotocol = 0;
		m_unGSSteamID = 0ull;;
		m_unLobbyID = 0ull;
		m_bGSSecure = false;
		m_bRequiresPassword = false;
		m_bDCFriendsReqd = false;
		m_bOfficialValveServer = false;
		m_nEncryptionKey = 0;
		m_nEncryptedSize = 0;
		memset(m_chLobbyType, 0, sizeof(m_chLobbyType));
	}

	bool	m_bActive;
	bool	m_bGSSecure;
	bool	m_bRequiresPassword;
	bool	m_bDCFriendsReqd;
	bool	m_bOfficialValveServer;
	char	m_chLobbyType[16];

	int		m_nChallenge;
	int		m_nAuthprotocol;
	uint64	m_unGSSteamID;
	uint64	m_unLobbyID;
	ns_address m_adrServerAddress;
	int		m_nEncryptionKey;
	int		m_nEncryptedSize;
};


typedef bool(__thiscall* ProcessConnectionlessFunc_t)(void* ecx, netpacket_s* packet);
bool __fastcall hk_ProcessConnectionless(void* ecx, void* edx, netpacket_s* pPacket) {
	bf_read msg(pPacket->data, pPacket->size);


	if (msg.ReadLong() != CONNECTIONLESS_HEADER)
	{
		CON(XorStr("Invalid Connectionless Packet Recieved\n"));
		return oProcessConnectionless(ecx, pPacket);
	}

	int c = msg.ReadByte();
	switch (c)
	{
	case S2C_CONNECTION:
#ifdef _DEBUG
		CON(XorStr("S2C_CONNECTION\n"));
#endif
		break;
	case S2C_CHALLENGE:
	{
#ifdef _DEBUG
		CON(XorStr("S2C_CHALLENGE\n"));
#endif

		int nChallenge = msg.ReadLong();
		VCON(XorStr("S2C_CHALLENGE with %d Challenge Number\n"), nChallenge);
		Globals::g_nLastChallengeNumber = nChallenge;

		int nAuthprotocol = msg.ReadLong();
		uint64_t unGSSteamID = 0;
		bool bGSSecure = false;
		if (nAuthprotocol == PROTOCOL_STEAM)
		{
			if (msg.ReadShort() != 0)
			{
				//Msg("Invalid Steam key size.\n");
				//Disconnect();
				return false;
			}
			unGSSteamID = msg.ReadLongLong();
			bGSSecure = msg.ReadByte() ? true : false;
		}
		else
		{
			msg.ReadShort();
			unGSSteamID = msg.ReadLongLong();	// still read out game server SteamID for token validation
			msg.ReadByte();
		}

		char context[256] = { 0 };
		msg.ReadString(context, sizeof(context));

		if (strstr(context, "connect"))
		{
			//g_nLastEncryptionKey
			int nProto = msg.ReadLong();
			Globals::g_nServerProtocolVersion = nProto;
			
			char chLobbyType[16];
			msg.ReadString(chLobbyType, 16 - 1);
			bool bRequiresPassword = (msg.ReadByte() != 0);
			uint64_t unLobbyID = msg.ReadLongLong();
			bool bDCFriendsReqd = (msg.ReadByte() != 0);
			bool bOfficialValveServer = (msg.ReadByte() != 0);
			bool bEncryptedChannel = (msg.ReadByte() != 0);

			if (bEncryptedChannel)
			{
				byte chKeyPub[1024] = {};
				byte chKeySgn[1024] = {};
				int cbKeyPub = msg.ReadLong();
				msg.ReadBytes(chKeyPub, cbKeyPub);
				int cbKeySgn = msg.ReadLong();
				msg.ReadBytes(chKeySgn, cbKeySgn);
				if (msg.IsOverflowed())
				{
					Msg("Invalid challenge packet.\n");
					//Disconnect();
					return false;
				}

#if 0
				// Verify server certificate signature
				byte* pbAllocatedKey = NULL;
				int nAllocatedCryptoBlockSize = 0;
				if (!NET_CryptVerifyServerCertificateAndAllocateSessionKey(bOfficialValveServer, adrServerAddress,
					chKeyPub, cbKeyPub, chKeySgn, cbKeySgn,
					&pbAllocatedKey, &nAllocatedCryptoBlockSize) || !pbAllocatedKey || !nAllocatedCryptoBlockSize)
				{
					delete[] pbAllocatedKey;
					Msg("Bad challenge signature.\n");
					//Disconnect();
					return false;
				}

				static int s_nGeneratedEncryptionKey = 0;
				++s_nGeneratedEncryptionKey;
				if (!s_nGeneratedEncryptionKey)
					++s_nGeneratedEncryptionKey;

				//m_mapGeneratedEncryptionKeys.InsertOrReplace(s_nGeneratedEncryptionKey, pbAllocatedKey);
				//dc.m_nEncryptionKey = s_nGeneratedEncryptionKey;
				//dc.m_nEncryptedSize = nAllocatedCryptoBlockSize;
#endif
			}


		}

		VCON(XorStr("S2C_Challenge Context : %s\n"), context);

		break;
	}
	case S2C_CONNREJECT:
#ifdef _DEBUG
		//CON(XorStr("S2C_CONNREJECT\n"));
#endif
		return true; // TODO: Ratelimit
		break;
	case S2A_RESERVE_RESPONSE:
#ifdef _DEBUG
		CON(XorStr("S2A_RESERVE_RESPONSE\n"));
#endif
		break;
	case S2A_RESERVE_CHECK_RESPONSE:
#ifdef _DEBUG
		CON(XorStr("S2A_RESERVE_CHECK_RESPONSE\n"));
#endif
		break;
	case S2A_PING_RESPONSE:
#ifdef _DEBUG
		CON(XorStr("S2A_PING_RESPONSE\n"));
#endif
		break;
	case S2A_INFO_SRC:
#ifdef _DEBUG
		CON(XorStr("S2A_INFO_SRC\n"));
#endif
		break;
	case 0:
	{

	//MemoryTools::HexPrint(XorStr("OnNetLanConnectionlessPacket"), pPacket->data, pPacket->size, 8);

	//if (g_bBlockP2PSessionRequests)
	//	return false;
	break;
	}
	default:
		VCON(XorStr("Unknown Connectionless Packet %c\n"), c);
#ifndef _DEBUG
		MemoryTools::HexPrint(XorStr("Connectionless Packet"), pPacket->data, pPacket->size, 8);
#endif
		break;
	}

#ifdef _DEBUG
	MemoryTools::HexPrint(XorStr("Connectionless Packet"), pPacket->data, pPacket->size, 8);
#endif


	return oProcessConnectionless(ecx, pPacket);
}

