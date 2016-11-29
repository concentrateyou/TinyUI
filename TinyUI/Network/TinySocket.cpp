#include "../stdafx.h"
#include "TinySocket.h"
#include <process.h>

namespace TinyUI
{
	namespace Network
	{
		IPAddress::IPAddress()
		{
			TRACE("IPAddress构造-0\n");
		}
		IPAddress::IPAddress(const std::vector<BYTE>& address)
			: m_address(std::move(address))
		{
			TRACE("IPAddress构造-1\n");
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
			TRACE("IPAddress拷贝构造\n");
		}
		IPAddress::IPAddress(IPAddress&& other)
			: m_address(std::move(other.m_address))
		{
			TRACE("IPAddress移动构造\n");
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
			TRACE("IPAddress析构\n");
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
		std::string IPAddress::ToString() const
		{
			return "";
		}
		DWORD IPAddress::Size() const
		{
			return static_cast<DWORD>(m_address.size());
		}
		const std::vector<BYTE>& IPAddress::Address() const
		{
			return m_address;
		}
		//////////////////////////////////////////////////////////////////////////
		PER_IO_CONTEXT::PER_IO_CONTEXT()
		{
			this->Internal = 0;
			this->InternalHigh = 0;
			this->Offset = 0;
			this->OffsetHigh = 0;
			this->Pointer = NULL;
			this->hEvent = NULL;
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
		//////////////////////////////////////////////////////////////////////////
		TinyPtrMap TinySocket::m_socketMap;

		TinySocket::TinySocket()
		{

		}
		TinySocket::~TinySocket()
		{
			Close();
		}
		TinySocket::operator SOCKET() const
		{
			return m_socket;
		}
		SOCKET TinySocket::Handle() const
		{
			return m_socket;
		}
		BOOL TinySocket::operator == (const TinySocket& obj) const
		{
			return m_socket == obj.m_socket;
		}
		BOOL TinySocket::operator != (const TinySocket& obj) const
		{
			return m_socket != obj.m_socket;
		}
		BOOL TinySocket::Attach(SOCKET socket)
		{
			if (socket == INVALID_SOCKET)
				return FALSE;
			m_socket = socket;
			TinyPtrMap& map = TinySocket::m_socketMap;
			map.Add((UINT_PTR)socket, (UINT_PTR)this);
			return TRUE;
		}
		SOCKET TinySocket::Detach()
		{
			SOCKET socket = m_socket;
			if (socket != INVALID_SOCKET)
			{
				TinyPtrMap& map = TinySocket::m_socketMap;
				map.Remove((UINT_PTR)socket);
			}
			m_socket = NULL;
			return socket;
		}
		TinySocket* TinySocket::Lookup(SOCKET socket)
		{
			TinyPtrMap& map = TinySocket::m_socketMap;
			UINT_PTR val = 0;
			if (!map.Lookup((UINT_PTR)socket, val))
				return NULL;
			return reinterpret_cast<TinySocket*>(val);
		}
		void TinySocket::Close()
		{
			if (m_socket != INVALID_SOCKET)
			{
				closesocket(m_socket);
				m_socket = NULL;
			}
		}
		BOOL TinySocket::Shutdown(INT how)
		{
			if (m_socket != INVALID_SOCKET)
			{
				return shutdown(m_socket, how) != SOCKET_ERROR;
			}
			return FALSE;
		}

		BOOL TinySocket::GetAcceptEx(SOCKET socket, LPFN_ACCEPTEX* target)
		{
			return GetExtensionPtr(socket, WSAID_ACCEPTEX, (void**)target);
		}
		BOOL TinySocket::GetConnectEx(SOCKET socket, LPFN_CONNECTEX* target)
		{
			return GetExtensionPtr(socket, WSAID_CONNECTEX, (void**)target);
		}
		BOOL TinySocket::GetAcceptExSockaddrs(SOCKET socket, LPFN_GETACCEPTEXSOCKADDRS* target)
		{
			return GetExtensionPtr(socket, WSAID_GETACCEPTEXSOCKADDRS, (void**)target);
		}
		BOOL TinySocket::GetDisconnectEx(SOCKET socket, LPFN_DISCONNECTEX* target)
		{
			return GetExtensionPtr(socket, WSAID_DISCONNECTEX, (void**)target);
		}
	}
}