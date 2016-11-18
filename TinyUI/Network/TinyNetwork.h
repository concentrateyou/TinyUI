#pragma once
#include "../Common//TinyCollection.h"
#include "../IO/TinyIO.h"
#include "../Common/TinyCallback.h"
#include "../IO/TinyIOCP.h"
#include "../IO/TinyTaskBase.h"
#include <WinSock2.h>
#include <mswsock.h>
#pragma comment(lib,"Mswsock.lib")
#pragma comment(lib,"Ws2_32.lib")

namespace TinyUI
{
	namespace Network
	{
		class IPAddress
		{
			enum : size_t { IPv4AddressSize = 4, IPv6AddressSize = 16 };
		public:
			IPAddress();
			explicit IPAddress(const vector<BYTE>& address);
			IPAddress(IPAddress&& other);
			IPAddress(const IPAddress& other);
			template <INT N>
			IPAddress(const BYTE(&address)[N])
				: IPAddress(address, N)
			{}
			IPAddress(const BYTE* address, DWORD size);
			IPAddress(BYTE b0, BYTE b1, BYTE b2, BYTE b3);
			IPAddress(BYTE b0,
				BYTE b1,
				BYTE b2,
				BYTE b3,
				BYTE b4,
				BYTE b5,
				BYTE b6,
				BYTE b7,
				BYTE b8,
				BYTE b9,
				BYTE b10,
				BYTE b11,
				BYTE b12,
				BYTE b13,
				BYTE b14,
				BYTE b15);
			~IPAddress();
			BOOL operator==(const IPAddress& other) const;
			BOOL operator!=(const IPAddress& other) const;
			BOOL operator<(const IPAddress& other) const;
			BOOL IsIPv4() const;
			BOOL IsIPv6() const;
			BOOL IsValid() const;
			BOOL IsZero() const;
			BOOL IsEmpty() const;
			std::string ToString() const;
			DWORD Size() const;
			const std::vector<BYTE>& Address() const;
		private:
			std::vector<uint8_t> m_address;
		};
		using IPAddressList = std::vector<IPAddress>;
		enum AddressFamily
		{
			ADDRESS_FAMILY_UNSPECIFIED,
			ADDRESS_FAMILY_IPV4,
			ADDRESS_FAMILY_IPV6,
			ADDRESS_FAMILY_LAST = ADDRESS_FAMILY_IPV6
		};
		//////////////////////////////////////////////////////////////////////////
		class IPEndPoint
		{
		public:
			IPEndPoint();
			~IPEndPoint();
			IPEndPoint(const IPAddress& address, USHORT port);
			IPEndPoint(const IPEndPoint& endpoint);
			IPEndPoint(IPEndPoint&& endpoint);
			const IPAddress& Address() const;
			USHORT Port() const;
		private:
			IPAddress	m_address;
			USHORT		m_port;
		};
	}
}

