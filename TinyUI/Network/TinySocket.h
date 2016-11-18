#pragma once
#include "TinyNetwork.h"

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
		typedef Callback<void(INT)> CompletionCallback;
		class TinySocket
		{
		public:
			static BOOL GetAcceptEx(SOCKET socket, LPFN_ACCEPTEX* target);
			static BOOL GetConnectEx(SOCKET socket, LPFN_CONNECTEX* target);
			static BOOL GetAcceptExSockaddrs(SOCKET socket, LPFN_GETACCEPTEXSOCKADDRS* target);
			static BOOL GetDisconnectEx(SOCKET socket, LPFN_DISCONNECTEX* target);
		public:
			TinySocket();
			virtual ~TinySocket();
			virtual INT Read(BYTE* data, INT size, const CompletionCallback& callback) = 0;
			virtual INT Write(BYTE* data, INT size, const CompletionCallback& callback) = 0;
		protected:
			LPFN_ACCEPTEX				m_acceptex;
			LPFN_CONNECTEX				m_connectex;
			LPFN_DISCONNECTEX			m_diconnectex;
			LPFN_GETACCEPTEXSOCKADDRS	m_getAcceptExSockaddrs;
		};
	}
}

