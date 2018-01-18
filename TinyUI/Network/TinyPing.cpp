#include "../stdafx.h"
#include "TinyPing.h"
#include <Ipexport.h>
#include <Icmpapi.h>
#pragma comment(lib,"Iphlpapi.lib")

namespace TinyUI
{
	namespace Network
	{
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
	
		BOOL TinyPing::Ping(IPAddress& address, WORD wRequestSize, DWORD dwTimeout, BYTE TTL, BOOL allow)
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
			DWORD dwRes = IcmpSendEcho(m_handle, addrIN->sin_addr.S_un.S_addr, m_request, wRequestSize, &ipo, m_replay, dwReplaySize, dwTimeout);
			return dwRes != 0;
		}
	}
}