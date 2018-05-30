#include "../stdafx.h"
#include "TinyPing.h"
#include <Ipexport.h>
#include <Icmpapi.h>
#pragma comment(lib,"Iphlpapi.lib")

namespace TinyFramework
{
	namespace Network
	{
		const UINT16 PACKET_MAXIMUMS[] =
		{
			65535,
			32000,
			17914,
			8166,
			4352,
			2002,
			1492,
			1006,
			508,
			296,
			68,
			0,
		};
		static const INT IP_HEADER_SIZE = 20;
		static const INT IPV6_HEADER_SIZE = 40;
		static const INT ICMP_HEADER_SIZE = 8;
		static const INT ICMP_PING_TIMEOUT_MILLIS = 10000;
		//////////////////////////////////////////////////////////////////////////
		TinyPing::TinyPing()
			:m_handle(NULL)
		{
			m_handle = IcmpCreateFile();
		}
		TinyPing::~TinyPing()
		{
			if (m_handle != NULL)
			{
				IcmpCloseHandle(m_handle);
				m_handle = NULL;
			}
		}
		BOOL TinyPing::IsEmpty() const
		{
			return (m_handle == NULL || m_handle == INVALID_HANDLE_VALUE);
		}

		DWORD TinyPing::Ping(IPAddress& address, WORD wRequestSize, DWORD dwTimeout, BYTE TTL, BOOL allow)
		{
			if (IsEmpty())
				return FALSE;
			IP_OPTION_INFORMATION ipo;
			memset(&ipo, 0, sizeof(ipo));
			ipo.Ttl = TTL;
			ipo.Flags |= !allow ? IP_FLAG_DF : 0;
			if (m_request.GetSize() < wRequestSize)
			{
				m_request.Reset(wRequestSize);
			}
			memset(m_request, 'z', m_request.GetSize());
			DWORD dwReplaySize = sizeof(ICMP_ECHO_REPLY) + (UINT32)max(8, wRequestSize);
			if (m_replay.GetSize() < dwReplaySize)
			{
				m_replay.Reset(dwReplaySize);
			}
			SOCKADDR addr = { 0 };
			SOCKADDR_IN* addrIN = reinterpret_cast<SOCKADDR_IN*>(&addr);
			memset(addrIN, 0, sizeof(struct sockaddr_in));
			addrIN->sin_family = AF_INET;
			memcpy(&addrIN->sin_addr, address.address().data(), IPAddress::IPv4AddressSize);
			return IcmpSendEcho(m_handle, addrIN->sin_addr.S_un.S_addr, m_request, wRequestSize, &ipo, m_replay, dwReplaySize, dwTimeout);
		}
		BOOL TinyPing::EstimateMTU(IPAddress& address, UINT16* pMTU)
		{
			INT headerSize = ICMP_HEADER_SIZE;
			if (address.IsIPv4())
			{
				headerSize += IPV6_HEADER_SIZE;
			}
			if (address.IsIPv6())
			{
				headerSize += IP_HEADER_SIZE;
			}
			for (INT i = 0; PACKET_MAXIMUMS[i + 1] > 0; ++i)
			{
				UINT32 size = PACKET_MAXIMUMS[i] - headerSize;
				DWORD dwRes = Ping(address, size, ICMP_PING_TIMEOUT_MILLIS, 1, FALSE);
				if (dwRes == 0)
				{
					dwRes = GetLastError();
					if (dwRes == IP_PACKET_TOO_BIG)
					{
						*pMTU = PACKET_MAXIMUMS[i];
						break;
					}
					return FALSE;
				}
			}
			return TRUE;
		}
	}
}