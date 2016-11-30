#include "../stdafx.h"
#include "TinySocket.h"
#include "TinyIOServer.h"

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
		IPAddress IPAddress::IPv4Any()
		{
			return IPAddress(0, 0, 0, 0);
		}
		IPAddress IPAddress::IPv6Any()
		{
			return IPAddress(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
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
			this->Buffer.buf = NULL;
			this->Buffer.len = 0;
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
		TinyPtrMap TinyHandleSOCKET::m_socketMap;

		TinyHandleSOCKET::TinyHandleSOCKET()
			:m_socket(INVALID_SOCKET)
		{

		}
		TinyHandleSOCKET::~TinyHandleSOCKET()
		{

		}
		TinyHandleSOCKET::operator SOCKET() const
		{
			return m_socket;
		}
		SOCKET TinyHandleSOCKET::Handle() const
		{
			return m_socket;
		}
		BOOL TinyHandleSOCKET::operator == (const TinyHandleSOCKET& obj) const
		{
			return m_socket == obj.m_socket;
		}
		BOOL TinyHandleSOCKET::operator != (const TinyHandleSOCKET& obj) const
		{
			return m_socket != obj.m_socket;
		}
		BOOL TinyHandleSOCKET::Attach(SOCKET socket)
		{
			if (socket == INVALID_SOCKET)
				return FALSE;
			m_socket = socket;
			TinyPtrMap& map = TinyHandleSOCKET::m_socketMap;
			map.Add((UINT_PTR)socket, (UINT_PTR)this);
			return TRUE;
		}
		SOCKET TinyHandleSOCKET::Detach()
		{
			SOCKET socket = m_socket;
			if (socket != INVALID_SOCKET)
			{
				TinyPtrMap& map = TinyHandleSOCKET::m_socketMap;
				map.Remove((UINT_PTR)socket);
			}
			m_socket = NULL;
			return socket;
		}
		TinyHandleSOCKET* TinyHandleSOCKET::Lookup(SOCKET socket)
		{
			TinyPtrMap& map = TinyHandleSOCKET::m_socketMap;
			UINT_PTR val = 0;
			if (!map.Lookup((UINT_PTR)socket, val))
				return NULL;
			return reinterpret_cast<TinyHandleSOCKET*>(val);
		}
		BOOL TinyHandleSOCKET::SetOption(INT opt, const CHAR* optval, INT size)
		{
			ASSERT(m_socket);
			return setsockopt(m_socket, SOL_SOCKET, opt, optval, size) != SOCKET_ERROR;
		}
		BOOL TinyHandleSOCKET::GetOption(INT opt, CHAR* optval, INT& size)
		{
			ASSERT(m_socket);
			return getsockopt(m_socket, SOL_SOCKET, opt, optval, &size) != SOCKET_ERROR;
		}
		BOOL TinyHandleSOCKET::GetAcceptEx(SOCKET socket, LPFN_ACCEPTEX* target)
		{
			return GetExtensionPtr(socket, WSAID_ACCEPTEX, (void**)target);
		}
		BOOL TinyHandleSOCKET::GetConnectEx(SOCKET socket, LPFN_CONNECTEX* target)
		{
			return GetExtensionPtr(socket, WSAID_CONNECTEX, (void**)target);
		}
		BOOL TinyHandleSOCKET::GetAcceptExSockaddrs(SOCKET socket, LPFN_GETACCEPTEXSOCKADDRS* target)
		{
			return GetExtensionPtr(socket, WSAID_GETACCEPTEXSOCKADDRS, (void**)target);
		}
		BOOL TinyHandleSOCKET::GetDisconnectEx(SOCKET socket, LPFN_DISCONNECTEX* target)
		{
			return GetExtensionPtr(socket, WSAID_DISCONNECTEX, (void**)target);
		}
		//////////////////////////////////////////////////////////////////////////
		TinySocket::TinySocket(TinyIOServer* ioserver)
			:m_server(ioserver),
			m_addressFamily(AF_INET),
			m_socketType(SOCK_STREAM),
			m_protocolType(IPPROTO_TCP)
		{

		}
		BOOL TinySocket::Open(INT addressFamily /* = AF_INET */, INT socketType /* = SOCK_STREAM */, INT protocolType /* = IPPROTO_TCP */)
		{
			ASSERT(m_server);
			m_addressFamily = addressFamily;
			m_socketType = socketType;
			m_protocolType = protocolType;
			m_socket = WSASocket(addressFamily, socketType, protocolType, NULL, 0, WSA_FLAG_OVERLAPPED);
			BOOL allow = TRUE;
			if (!SetOption(SO_REUSEADDR, (const CHAR*)&allow, sizeof(allow)))
				return FALSE;
			if (!Attach(m_socket))
				return FALSE;
			return m_server->GetIOCP()->Register(reinterpret_cast<HANDLE>(m_socket), m_socket);
		}
		TinySocket::~TinySocket()
		{
			Close();
		}
		BOOL TinySocket::Bind(const IPAddress& address, DWORD dwPORT)
		{
			ASSERT(m_socket);
			SOCKADDR_IN si;
			ZeroMemory(&si, sizeof(si));
			si.sin_family = m_addressFamily;
			si.sin_port = htons(static_cast<USHORT>(dwPORT));
			memcpy(&si.sin_addr, address.Address().data(), m_addressFamily == AF_INET ? IPAddress::IPv4AddressSize : IPAddress::IPv6AddressSize);
			return bind(m_socket, (SOCKADDR*)&si, sizeof(si)) != SOCKET_ERROR;
		}
		BOOL TinySocket::Listen(DWORD backlog)
		{
			ASSERT(m_socket);
			return listen(m_socket, SOMAXCONN) != SOCKET_ERROR;
		}
		BOOL TinySocket::BeginAccept(CompleteCallback& callback)
		{
			ASSERT(m_server);
			TinySocket* socket = new TinySocket(m_server);
			if (socket && !socket->Open(m_addressFamily, m_socketType, m_protocolType))
				goto _ERROR;
			LPFN_ACCEPTEX acceptex = NULL;
			if (!TinySocket::GetAcceptEx(m_socket, &acceptex))
				goto _ERROR;
			ZeroMemory(&socket->m_context, sizeof(PER_IO_CONTEXT));
			socket->m_context.OP = OP_ACCEPT;
			socket->m_context.Key = reinterpret_cast<LONG_PTR>(socket);
			socket->m_context.Complete = std::move(callback);
			DWORD dwBytes = 0;
			DWORD dwAddressSize = sizeof(SOCKADDR_IN) + 16;
			CHAR data[(sizeof(SOCKADDR_IN) + 16) * 2];
			ZeroMemory(data, (sizeof(SOCKADDR_IN) + 16) * 2);
			LPOVERLAPPED ps = static_cast<LPOVERLAPPED>(&socket->m_context);
			if (!acceptex(m_socket, socket->Handle(), data, 0, dwAddressSize, dwAddressSize, &dwBytes, ps) &&
				ERROR_IO_PENDING != WSAGetLastError())
			{
				goto _ERROR;
			}
			return TRUE;
		_ERROR:
			SAFE_DELETE(socket);
			return FALSE;
		}
		BOOL TinySocket::BeginConnect(IPAddress& address, DWORD dwPORT, CompleteCallback& callback)
		{
			//https://msdn.microsoft.com/en-us/library/windows/desktop/ms737606(v=vs.85).aspx
			if (!Open(m_addressFamily, m_socketType, m_protocolType))
				goto _ERROR;
			SOCKADDR_IN si;
			ZeroMemory(&si, sizeof(si));
			si.sin_family = m_addressFamily;
			si.sin_port = 0;
			si.sin_addr.s_addr = htonl(INADDR_ANY);
			if (bind(m_socket, (SOCKADDR*)&si, sizeof(si)) == SOCKET_ERROR)
				goto _ERROR;
			ZeroMemory(&m_context, sizeof(m_context));
			m_context.OP = OP_CONNECT;
			m_context.Key = reinterpret_cast<LONG_PTR>(this);
			m_context.Complete = std::move(callback);
			ZeroMemory(&si, sizeof(si));
			si.sin_family = AF_INET;
			si.sin_port = htons(static_cast<USHORT>(dwPORT));
			memcpy(&si.sin_addr, address.Address().data(), m_addressFamily == AF_INET ? IPAddress::IPv4AddressSize : IPAddress::IPv6AddressSize);
			LPFN_CONNECTEX connectex = NULL;
			if (!TinySocket::GetConnectEx(m_socket, &connectex))
				goto _ERROR;
			LPOVERLAPPED ps = static_cast<LPOVERLAPPED>(&m_context);
			if (!connectex(m_socket, (SOCKADDR*)&si, sizeof(si), NULL, 0, NULL, ps) &&
				ERROR_IO_PENDING != WSAGetLastError())
			{
				goto _ERROR;
			}
			return TRUE;
		_ERROR:
			Close();
			return FALSE;
		}
		BOOL TinySocket::BeginReceive(BYTE* data, INT size, CompleteCallback& callback)
		{
			ASSERT(m_socket);
			if (!m_connect)
				return FALSE;
			return TRUE;
		}
		BOOL TinySocket::BeginDisconnect(CompleteCallback& callback)
		{
			LPFN_DISCONNECTEX disconnectex = NULL;
			if (!TinySocket::GetDisconnectEx(m_socket, &disconnectex))
				goto _ERROR;

		_ERROR:
			Close();
			return FALSE;
		}
		BOOL TinySocket::IsConnect() const
		{
			return m_connect;
		}
		void TinySocket::Close()
		{
			SOCKET socket = Detach();
			if (socket != INVALID_SOCKET)
			{
				closesocket(socket);
				socket = NULL;
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
	}
}