#include "../stdafx.h"
#include "TinyNetwork.h"

namespace TinyUI
{
	namespace Network
	{
		BOOL GetExtensionPtr(SOCKET socket, GUID guid, void **target)
		{
			DWORD dwBytes = 0;
			if (WSAIoctl(socket,
				SIO_GET_EXTENSION_FUNCTION_POINTER,
				&guid,
				sizeof(guid),
				(void*)target,
				sizeof(*target),
				&dwBytes,
				NULL,
				NULL) != S_OK)
			{
				*target = NULL;
				return FALSE;
			}
			return TRUE;
		}
		DWORD GetErrorCode(SOCKET socket, LPOVERLAPPED ps)
		{
			ASSERT(socket);
			DWORD dwBytes = 0;
			DWORD dwFlag = 0;
			if (WSAGetOverlappedResult(socket, ps, &dwBytes, FALSE, &dwFlag))
				return ERROR_SUCCESS;
			return WSAGetLastError();
		}
		void AppendIPv4Address(const BYTE address[4], string* s)
		{
			for (INT i = 0; i < 4; i++)
			{
				CHAR str[16];
				_itoa_s(address[i], str, 10);
				for (INT ch = 0; str[ch] != 0; ch++)
					s->append(1, str[ch]);
				if (i != 3)
					s->append(1, '.');
			}
		}
		BOOL GetIPAddressFromSOCKADDR(const SOCKADDR* address, INT addresssize, const BYTE** addressdata, size_t* datasize, USHORT* port)
		{
			if (address->sa_family == AF_INET)
			{
				if (addresssize < static_cast<INT>(sizeof(SOCKADDR_IN)))
					return FALSE;
				const SOCKADDR_IN* addr = reinterpret_cast<const SOCKADDR_IN*>(address);
				*addressdata = reinterpret_cast<const BYTE*>(&addr->sin_addr);
				*datasize = IPAddress::IPv4AddressSize;
				if (port)
				{
					*port = ntohs(addr->sin_port);
				}
				return TRUE;
			}
			if (address->sa_family == AF_INET6)
			{
				if (addresssize < static_cast<INT>(sizeof(SOCKADDR_IN6)))
					return FALSE;
				const SOCKADDR_IN6* addr = reinterpret_cast<const SOCKADDR_IN6*>(address);
				*addressdata = reinterpret_cast<const BYTE*>(&addr->sin6_addr);
				*datasize = IPAddress::IPv6AddressSize;
				if (port)
					*port = ntohs(addr->sin6_port);
				return TRUE;
			}
			return FALSE;
		}
		const INT SOCKADDR_IN_SIZE = sizeof(SOCKADDR_IN);
		const INT SOCKADDR_IN6_SIZE = sizeof(SOCKADDR_IN6);
		//////////////////////////////////////////////////////////////////////////
		IPAddress::IPAddress()
		{

		}
		IPAddress::IPAddress(const std::vector<BYTE>& address)
			: m_address(std::move(address))
		{

		}
		IPAddress::IPAddress(const string& ip)
		{
			ULONG si = inet_addr(ip.c_str());
			m_address.reserve(4);
			m_address.push_back((BYTE)(si & 0xFF));
			m_address.push_back((BYTE)(si >> 8 & 0xFF));
			m_address.push_back((BYTE)(si >> 16 & 0xFF));
			m_address.push_back((BYTE)(si >> 24 & 0xFF));
		}
		IPAddress::IPAddress(const IPAddress& other)
			: m_address(std::move(other.m_address))
		{

		}
		IPAddress::IPAddress(IPAddress&& other)
			: m_address(std::move(other.m_address))
		{

		}
		IPAddress::IPAddress(const BYTE* address, DWORD size)
			: m_address(address, address + size)
		{
		}
		IPAddress::IPAddress(BYTE b0, BYTE b1, BYTE b2, BYTE b3)
		{
			m_address.reserve(4);
			m_address.push_back(b0);
			m_address.push_back(b1);
			m_address.push_back(b2);
			m_address.push_back(b3);
		}
		IPAddress::IPAddress(BYTE b0,
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
			BYTE b15)
		{
			const BYTE address[] = { b0, b1, b2,  b3,  b4,  b5,  b6,  b7, b8, b9, b10, b11, b12, b13, b14, b15 };
			m_address = std::vector<BYTE>(std::begin(address), std::end(address));
		}
		IPAddress& IPAddress::operator =(const IPAddress& other)
		{
			m_address = std::move(other.m_address);
			return *this;
		}
		BOOL IPAddress::operator==(const IPAddress& other) const
		{
			return m_address == other.m_address;
		}
		BOOL IPAddress::operator!=(const IPAddress& other) const
		{
			return m_address != other.m_address;
		}
		BOOL IPAddress::operator<(const IPAddress& that) const
		{
			if (m_address.size() != that.m_address.size())
			{
				return m_address.size() < that.m_address.size();
			}
			return m_address < that.m_address;
		}
		IPAddress::~IPAddress()
		{

		}
		BOOL IPAddress::IsIPv4() const
		{
			return m_address.size() == IPv4AddressSize;
		}
		BOOL IPAddress::IsIPv6() const
		{
			return m_address.size() == IPv6AddressSize;
		}
		BOOL IPAddress::IsValid() const
		{
			return IsIPv4() || IsIPv6();
		}
		BOOL IPAddress::IsZero() const
		{
			for (auto x : m_address)
			{
				if (x != 0)
				{
					return FALSE;
				}
			}
			return !IsEmpty();
		}
		BOOL IPAddress::IsEmpty() const
		{
			return m_address.empty();
		}
		INT	IPAddress::AddressFamily() const
		{
			if (IsIPv4())
				return AF_INET;
			if (IsIPv6())
				return AF_INET6;
			return AF_UNSPEC;
		}
		std::string IPAddress::ToString() const
		{
			std::string val;
			if (IsIPv4())
			{
				AppendIPv4Address(m_address.data(), &val);
			}
			else if (IsIPv6())
			{
				//TODO
			}
			return val;
		}
		size_t IPAddress::size() const
		{
			return m_address.size();
		}
		const std::vector<BYTE>& IPAddress::address() const
		{
			return m_address;
		}
		IPAddress IPAddress::IPv4Any()
		{
			return IPAddress(0, 0, 0, 0);
		}
		IPAddress IPAddress::IPv6Any()
		{
			return IPAddress(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		}
		//////////////////////////////////////////////////////////////////////////
		IPEndPoint::IPEndPoint()
			:m_port(0)
		{

		}
		IPEndPoint::~IPEndPoint()
		{

		}
		IPEndPoint::IPEndPoint(const IPAddress& address, USHORT port)
			:m_address(address),
			m_port(port)
		{

		}
		IPEndPoint::IPEndPoint(const IPEndPoint& endpoint)
		{
			m_address = endpoint.m_address;
			m_port = endpoint.m_port;
		}
		const IPAddress& IPEndPoint::address() const
		{
			return m_address;
		}
		INT	IPEndPoint::AddressFamily() const
		{
			return m_address.AddressFamily();
		}
		USHORT IPEndPoint::port() const
		{
			return m_port;
		}
		BOOL IPEndPoint::ToSOCKADDR(SOCKADDR* address, size_t* size) const
		{
			switch (m_address.size())
			{
			case IPAddress::IPv4AddressSize:
			{
				if (*size < SOCKADDR_IN_SIZE)
					return FALSE;
				*size = SOCKADDR_IN_SIZE;
				SOCKADDR_IN* addr = reinterpret_cast<SOCKADDR_IN*>(address);
				memset(addr, 0, sizeof(struct sockaddr_in));
				addr->sin_family = AF_INET;
				addr->sin_port = htons(m_port);
				memcpy(&addr->sin_addr, m_address.address().data(), IPAddress::IPv4AddressSize);
				break;
			}
			case IPAddress::IPv6AddressSize:
			{
				if (*size < SOCKADDR_IN6_SIZE)
					return FALSE;
				*size = SOCKADDR_IN6_SIZE;
				SOCKADDR_IN6* addr6 = reinterpret_cast<SOCKADDR_IN6*>(address);
				memset(addr6, 0, SOCKADDR_IN6_SIZE);
				addr6->sin6_family = AF_INET6;
				addr6->sin6_port = htons(m_port);
				memcpy(&addr6->sin6_addr, m_address.address().data(), IPAddress::IPv6AddressSize);
				break;
			}
			default:
				return FALSE;
			}
			return TRUE;
		}
		BOOL IPEndPoint::FromSOCKADDR(const SOCKADDR* address, size_t size)
		{
			const BYTE* addressdata = NULL;
			size_t datasize = 0;
			USHORT port = 0;
			if (!GetIPAddressFromSOCKADDR(address, size, &addressdata, &datasize, &port))
				return FALSE;
			m_address = IPAddress(addressdata, datasize);
			m_port = port;
			return TRUE;
		}
		std::string IPEndPoint::ToString() const
		{
			return StringPrintf("%s,%d", m_address.ToString().c_str(), m_port);
		}
		BOOL IPEndPoint::operator<(const IPEndPoint& other) const
		{
			if (m_address.size() != other.m_address.size())
			{
				return m_address.size() < other.m_address.size();
			}
			return std::tie(m_address, m_port) < std::tie(other.m_address, other.m_port);
		}
		BOOL IPEndPoint::operator==(const IPEndPoint& other) const
		{
			return m_address == other.m_address && m_port == other.m_port;
		}
		//////////////////////////////////////////////////////////////////////////
		AsyncResult::AsyncResult()
			:AsyncState(NULL),
			AsyncHandle(NULL)
		{

		}
		AsyncResult::~AsyncResult()
		{

		}
		PER_IO_CONTEXT::PER_IO_CONTEXT()
		{
			Reset();
		}
		PER_IO_CONTEXT::~PER_IO_CONTEXT()
		{

		}
		void PER_IO_CONTEXT::Reset()
		{
			this->Internal = 0;
			this->InternalHigh = 0;
			this->Offset = 0;
			this->OffsetHigh = 0;
			this->Pointer = NULL;
			this->hEvent = NULL;
		}
	}
}