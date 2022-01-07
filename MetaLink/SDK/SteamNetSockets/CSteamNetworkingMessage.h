#pragma once

#include "../BaseIncludes.h"
#include "mathlib/mathlib.h"
#include "IMaterialSystem.h"

#include <steam/isteamnetworkingsockets.h>


#define assert(VAR) _ASSERT(VAR)

#if 0
class CSteamNetworkingMessage {

    int32_t nUnknown;
    int32_t m_nCbSize;
    char m_pad[180];
    int8_t m_nFlags;
    char m_pad2[11];
    int32_t m_cbSNPSendReliableHeader; /* Size Of Header */
};
#endif

#ifdef STEAMNETWORKINGSOCKETS_ENABLE_MEM_OVERRIDE
#define STEAMNETWORKINGSOCKETS_DECLARE_CLASS_OPERATOR_NEW \
		static void* operator new( size_t s ) noexcept { return malloc( s ); } \
		static void* operator new[]( size_t ) = delete; \
		static void operator delete( void *p ) noexcept { free( p ); } \
		static void operator delete[]( void * ) = delete;
#else
#define STEAMNETWORKINGSOCKETS_DECLARE_CLASS_OPERATOR_NEW
#endif
typedef unsigned char byte;
class SteamNetworkingMessageQueue;
class CSteamNetworkConnectionBase;
class CSteamNetworkingMessage : public SteamNetworkingMessage_t
{
public:
    //STEAMNETWORKINGSOCKETS_DECLARE_CLASS_OPERATOR_NEW
    static CSteamNetworkingMessage* New(CSteamNetworkConnectionBase* pParent, uint32 cbSize, int64 nMsgNum, int nFlags, SteamNetworkingMicroseconds usecNow);
    static CSteamNetworkingMessage* New(uint32 cbSize);
    static void DefaultFreeData(SteamNetworkingMessage_t* pMsg);

    /// OK to delay sending this message until this time.  Set to zero to explicitly force
    /// Nagle timer to expire and send now (but this should behave the same as if the
    /// timer < usecNow).  If the timer is cleared, then all messages with lower message numbers
    /// are also cleared.
    // NOTE: Intentionally reusing the m_usecTimeReceived field, which is not used on outbound messages
    inline SteamNetworkingMicroseconds SNPSend_UsecNagle() const { return m_usecTimeReceived; }
    inline void SNPSend_SetUsecNagle(SteamNetworkingMicroseconds x) { m_usecTimeReceived = x; }

    /// Offset in reliable stream of the header byte.  0 if we're not reliable.
    inline int64 SNPSend_ReliableStreamPos() const { return m_nConnUserData; }
    inline void SNPSend_SetReliableStreamPos(int64 x) { m_nConnUserData = x; }
    inline int SNPSend_ReliableStreamSize() const
    {

        return m_cbSize;
    }

    inline bool SNPSend_IsReliable() const
    {
        if (m_nFlags & k_nSteamNetworkingSend_Reliable)
        {

            return true;
        }

        return false;
    }

    // Reliable stream header
    int m_cbSNPSendReliableHeader;
    byte* SNPSend_ReliableHeader()
    {
        // !KLUDGE! Reuse the peer identity to hold the reliable header
        return (byte*)&m_identityPeer;
    }

    /// Remove it from queues
    void Unlink();

    struct Links
    {
        SteamNetworkingMessageQueue* m_pQueue;
        CSteamNetworkingMessage* m_pPrev;
        CSteamNetworkingMessage* m_pNext;

        inline void Clear() { m_pQueue = nullptr; m_pPrev = nullptr; m_pNext = nullptr; }
    };

    /// Intrusive links for the "primary" list we are in
    Links m_links;

    /// Intrusive links for any secondary list we may be in.  (Same listen socket or
    /// P2P channel, depending on message type)
    Links m_linksSecondaryQueue;

    void LinkBefore(CSteamNetworkingMessage* pSuccessor, Links CSteamNetworkingMessage::* pMbrLinks, SteamNetworkingMessageQueue* pQueue);
    void LinkToQueueTail(Links CSteamNetworkingMessage::* pMbrLinks, SteamNetworkingMessageQueue* pQueue);
    void UnlinkFromQueue(Links CSteamNetworkingMessage::* pMbrLinks);

private:
    // Use New and Release()!!
    inline CSteamNetworkingMessage() {}
    inline ~CSteamNetworkingMessage() {}
    static void ReleaseFunc(SteamNetworkingMessage_t* pIMsg);
};


struct EncodedSegment
{
    static constexpr int k_cbMaxHdr = 16;
    uint8 m_hdr[k_cbMaxHdr];
    int m_cbHdr; // Doesn't include any size byte
    CSteamNetworkingMessage* m_pMsg;
    int m_cbSegSize;
    int m_nOffset;
};



struct SSendRateData
{
    /// Current sending rate in bytes per second, RFC 3448 4.2 states default
    /// is one packet per second, but that is insane and we're not doing that.
    /// In most cases we will set a default based on initial ping, so this is
    /// only rarely used.
    int m_nCurrentSendRateEstimate = 64 * 1024;

    /// Actual send rate we are going to USE.  This depends on the send rate estimate
    /// and the current BBR state
    float m_flCurrentSendRateUsed = 64 * 1024;

    /// If >=0, then we can send a full packet right now.  We allow ourselves to "store up"
    /// about 1 packet worth of "reserve".  In other words, if we have not sent any packets
    /// for a while, basically we allow ourselves to send two packets in rapid succession,
    /// thus "bursting" over the limit by 1 packet.  That long term rate will be clamped by
    /// the send rate.
    ///
    /// If <0, then we are currently "over" our rate limit and need to wait before we can
    /// send a packet.
    ///
    /// Provision for accumulating "credits" and burst allowance, to account for lossy
    /// kernel scheduler, etc is mentioned in RFC 5348, section 4.6.
    float m_flTokenBucket = 0;

    /// Last time that we added tokens to m_flTokenBucket
    SteamNetworkingMicroseconds m_usecTokenBucketTime = 0;

    /// Calculate time until we could send our next packet, checking our token
    /// bucket and the current send rate
    SteamNetworkingMicroseconds CalcTimeUntilNextSend() const
    {
        // Do we have tokens to burn right now?
        if (m_flTokenBucket >= 0.0f)
            return 0;

        return SteamNetworkingMicroseconds(m_flTokenBucket * -1e6f / m_flCurrentSendRateUsed) + 1; // +1 to make sure that if we don't have any tokens, we never return 0, since zero means "ready right now"
    }
};

namespace vstd
{
	template< typename T, int N >
	class small_vector
	{
	public:
		small_vector() {}
		small_vector(const small_vector<T, N>& x);
		small_vector<T, N>& operator=(const small_vector<T, N>& x);
		small_vector(small_vector<T, N>&& x);
		small_vector<T, N>& operator=(small_vector<T, N>&& x);
		~small_vector() { clear(); }

		size_t size() const { return size_; }
		size_t capacity() const { return capacity_; }
		bool empty() const { return size_ == 0; }

		T* begin() { return dynamic_ ? dynamic_ : (T*)fixed_; };
		const T* begin() const { return dynamic_ ? dynamic_ : (T*)fixed_; };

		T* end() { return begin() + size_; }
		const T* end() const { return begin() + size_; }

		T& operator[](size_t index) { assert(index < size_); return begin()[index]; }
		const T& operator[](size_t index) const { assert(index < size_); return begin()[index]; }

		void push_back(const T& value);
		void pop_back();
		void erase(T* it);

		void resize(size_t n);
		void reserve(size_t n);
		void clear();
		void assign(const T* srcBegin, const T* srcEnd);

	private:
		size_t size_ = 0, capacity_ = N;
		T* dynamic_ = nullptr;
		char fixed_[N][sizeof(T)];
	};

}

struct SNPRange_t
{
	/// Byte or sequence number range
	int64 m_nBegin;
	int64 m_nEnd; // STL-style.  It's one past the end


	struct NonOverlappingLess
	{
		inline bool operator ()(const SNPRange_t& l, const SNPRange_t& r) const
		{
			if (l.m_nBegin < r.m_nBegin) return true;
			return false;
		}
	};
};

class CConnectionTransport;
struct SNPInFlightPacket_t
{
	//
	// FIXME - Could definitely pack this structure better.  And maybe
	//         worth it to optimize cache
	//

	/// Local timestamp when we sent it
	SteamNetworkingMicroseconds m_usecWhenSent;

	/// Did we get an ack block from peer that explicitly marked this
	/// packet as being skipped?  Note that we might subsequently get an
	/// an ack for this same packet, that's OK!
	bool m_bNack;

	/// Transport used to send
	CConnectionTransport* m_pTransport;

	/// List of reliable segments.  Ignoring retransmission,
	/// there really is no reason why we we would need to have
	/// more than 1 in a packet, even if there are multiple
	/// reliable messages.  If we need to retry, we might
	/// be fragmented.  But usually it will only be a few.
	vstd::small_vector<SNPRange_t, 1> m_vecReliableSegments;
};


struct SteamNetworkingMessageQueue
{
	CSteamNetworkingMessage* m_pFirst = nullptr;
	CSteamNetworkingMessage* m_pLast = nullptr;
	void* m_pRequiredLock = nullptr; // Is there a lock that is required to be held while we access this queue?
};

struct SSNPSendMessageList : public SteamNetworkingMessageQueue
{
	CSteamNetworkingMessage* pop_front()
	{
		CSteamNetworkingMessage* pResult = m_pFirst;
		if (pResult)
		{
			//Assert(m_pLast);
			//Assert(pResult->m_links.m_pQueue == this);
			//Assert(pResult->m_links.m_pPrev == nullptr);
			m_pFirst = pResult->m_links.m_pNext;
			if (m_pFirst)
			{
				//Assert(m_pFirst->m_links.m_pPrev == pResult);
				//Assert(m_pFirst->m_nMessageNumber > pResult->m_nMessageNumber);
				m_pFirst->m_links.m_pPrev = nullptr;
			}
			else
			{
				//Assert(m_pLast == pResult);
				m_pLast = nullptr;
			}
			pResult->m_links.m_pQueue = nullptr;
			pResult->m_links.m_pNext = nullptr;
		}
		return pResult;
	}

	/// Optimized insertion when we know it goes at the end
	void push_back(CSteamNetworkingMessage* pMsg)
	{
		if (m_pFirst == nullptr)
		{
			//Assert(m_pLast == nullptr);
			m_pFirst = pMsg;
		}
		else
		{
			// Messages are always kept in message number order
			//Assert(pMsg->m_nMessageNumber > m_pLast->m_nMessageNumber);
			//Assert(m_pLast->m_links.m_pNext == nullptr);
			m_pLast->m_links.m_pNext = pMsg;
		}
		pMsg->m_links.m_pQueue = this;
		pMsg->m_links.m_pNext = nullptr;
		pMsg->m_links.m_pPrev = m_pLast;
		m_pLast = pMsg;
	}


};
struct SSNPSenderState
{
	SSNPSenderState();
	~SSNPSenderState() {
		Shutdown();
	}
	void Shutdown();

	/// Nagle timer on all pending messages
	void ClearNagleTimers()
	{
		CSteamNetworkingMessage* pMsg = m_messagesQueued.m_pLast;
		while (pMsg && pMsg->SNPSend_UsecNagle())
		{
			pMsg->SNPSend_SetUsecNagle(0);
			pMsg = pMsg->m_links.m_pPrev;
		}
	}

	// Current message number, we ++ when adding a message
	int64 m_nReliableStreamPos = 1;
	int64 m_nLastSentMsgNum = 0; // Will increment to 1 with first message
	int64 m_nLastSendMsgNumReliable = 0;
	SSNPSendMessageList m_messagesQueued;
	int m_cbCurrentSendMessageSent = 0;
	SSNPSendMessageList m_unackedReliableMessages;

	// Buffered data counters.  See SteamNetworkingQuickConnectionStatus for more info
	int m_cbPendingUnreliable = 0;
	int m_cbPendingReliable = 0;
	int m_cbSentUnackedReliable = 0;
	inline int PendingBytesTotal() const { return m_cbPendingUnreliable + m_cbPendingReliable; }

	// Stats.  FIXME - move to LinkStatsEndToEnd and track rate counters
	int64 m_nMessagesSentReliable = 0;
	int64 m_nMessagesSentUnreliable = 0;
#if 0
	std::map<int64, SNPInFlightPacket_t> m_mapInFlightPacketsByPktNum;
	std::map<int64, SNPInFlightPacket_t>::iterator m_itNextInFlightPacketToTimeout;
	std::map<SNPRange_t, CSteamNetworkingMessage*, SNPRange_t::NonOverlappingLess> m_listInFlightReliableRange;
	std::map<SNPRange_t, CSteamNetworkingMessage*, SNPRange_t::NonOverlappingLess> m_listReadyRetryReliableRange;
	int64 m_nMinPktWaitingOnAck = 0;
#endif
	/// Check invariants in debug.
#if STEAMNETWORKINGSOCKETS_SNP_PARANOIA == 0 
	//inline void DebugCheckInFlightPacketMap() const {}
#else
	void DebugCheckInFlightPacketMap() const;
#endif
#if STEAMNETWORKINGSOCKETS_SNP_PARANOIA > 1
	inline void MaybeCheckInFlightPacketMap() const { DebugCheckInFlightPacketMap(); }
#else
	//inline void MaybeCheckInFlightPacketMap() const {}
#endif

};