#include "../stdafx.h"
#include "TinySocket.h"
#include "TinyIOServer.h"

namespace TinyUI
{
	namespace Network
	{
		IPAddress::IPAddress()
		{
			//TRACE("IPAddress构造-0\n");
		}
		IPAddress::IPAddress(const std::vector<BYTE>& address)
			: m_address(std::move(address))
		{
			//TRACE("IPAddress构造-1\n");
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
			//TRACE("IPAddress拷贝构造\n");
		}
		IPAddress::IPAddress(IPAddress&& other)
			: m_address(std::move(other.m_address))
		{
			//TRACE("IPAddress移动构造\n");
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
			//TRACE("IPAddress析构\n");
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
			this->Element.buf = NULL;
			this->Element.len = 0;
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
		BOOL TinyHandleSOCKET::SetOption(INT level, INT opt, const CHAR* optval, INT size)
		{
			ASSERT(m_socket);
			return setsockopt(m_socket, level, opt, optval, size) != SOCKET_ERROR;
		}
		BOOL TinyHandleSOCKET::GetOption(INT level, INT opt, CHAR* optval, INT& size)
		{
			ASSERT(m_socket);
			return getsockopt(m_socket, level, opt, optval, &size) != SOCKET_ERROR;
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
			m_protocolType(IPPROTO_TCP),
			m_disconnectex(NULL),
			m_acceptex(NULL),
			m_connectex(NULL)
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
			if (!SetOption(SOL_SOCKET, SO_REUSEADDR, (const CHAR*)&allow, sizeof(allow)))
				return FALSE;
			if (!Attach(m_socket))
				return FALSE;
			return m_server->GetIOCP()->Register(reinterpret_cast<HANDLE>(m_socket), m_socket);
		}
		BOOL TinySocket::KeepAlive(BOOL bAllow)
		{
			ASSERT(m_socket);
			return SetOption(SOL_SOCKET, SO_KEEPALIVE, (const CHAR*)bAllow, sizeof(bAllow));
		}
		BOOL TinySocket::IsKeepAlive()
		{
			ASSERT(m_socket);
			BOOL bAllow = FALSE;
			INT size = 0;
			return GetOption(SOL_SOCKET, SO_KEEPALIVE, (CHAR*)bAllow, size);
		}
		INT	TinySocket::Available()
		{
			ASSERT(m_socket);
			INT argp = 0;
			if (ioctlsocket(m_socket, FIONREAD, (ULONG*)&argp) == SOCKET_ERROR)
				throw error_code(WSAGetLastError(), system_category());
			return argp;
		}
		BOOL TinySocket::Blocking(BOOL bAllow)
		{
			ASSERT(m_socket);
			return ioctlsocket(m_socket, FIONBIO, (ULONG*)&bAllow) != SOCKET_ERROR;
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
		BOOL TinySocket::BeginAccept(CompleteCallback& callback, LPVOID arg)
		{
			ASSERT(m_server);
			TinySocket* socket = new TinySocket(m_server);
			if (socket && !socket->Open(m_addressFamily, m_socketType, m_protocolType))
				goto _ERROR;
			if (!m_acceptex)
			{
				if (!TinySocket::GetAcceptEx(m_socket, &m_acceptex))
					goto _ERROR;
			}
			PER_IO_CONTEXT* context = new PER_IO_CONTEXT();
			ZeroMemory(context, sizeof(PER_IO_CONTEXT));
			context->OP = OP_ACCEPT;
			context->AsyncState = arg;
			context->Complete = std::move(callback);
			DWORD dwBytes = 0;
			DWORD dwAddressSize = sizeof(SOCKADDR_IN) + 16;
			CHAR data[(sizeof(SOCKADDR_IN) + 16) * 2];
			ZeroMemory(data, (sizeof(SOCKADDR_IN) + 16) * 2);
			LPOVERLAPPED ps = static_cast<LPOVERLAPPED>(context);
			if (!m_acceptex(m_socket, socket->Handle(), data, 0, dwAddressSize, dwAddressSize, &dwBytes, ps) &&
				ERROR_IO_PENDING != WSAGetLastError())
			{
				goto _ERROR;
			}
			return TRUE;
		_ERROR:
			DWORD errorCode = WSAGetLastError();
			if (!callback.IsNull())
				callback(errorCode, 0, &context->Address, arg);
			SAFE_DELETE(context);
			SAFE_DELETE(socket);
			return FALSE;
		}
		BOOL TinySocket::BeginConnect(IPAddress& address, DWORD dwPORT, CompleteCallback& callback, LPVOID arg)
		{
			//https://msdn.microsoft.com/en-us/library/windows/desktop/ms737606(v=vs.85).aspx
			DWORD errorCode = 0;
			if (!Open(m_addressFamily, m_socketType, m_protocolType))
				goto _ERROR;
			SOCKADDR_IN si;
			ZeroMemory(&si, sizeof(si));
			si.sin_family = m_addressFamily;
			si.sin_port = 0;
			si.sin_addr.s_addr = htonl(INADDR_ANY);
			if (bind(m_socket, (SOCKADDR*)&si, sizeof(si)) == SOCKET_ERROR)
				goto _ERROR;
			PER_IO_CONTEXT* context = new PER_IO_CONTEXT();
			ZeroMemory(context, sizeof(PER_IO_CONTEXT));
			context->OP = OP_CONNECT;
			context->AsyncState = arg;
			context->Complete = std::move(callback);
			ZeroMemory(&si, sizeof(si));
			si.sin_family = AF_INET;
			si.sin_port = htons(static_cast<USHORT>(dwPORT));
			memcpy(&si.sin_addr, address.Address().data(), m_addressFamily == AF_INET ? IPAddress::IPv4AddressSize : IPAddress::IPv6AddressSize);
			if (!m_connectex)
			{
				if (!TinySocket::GetConnectEx(m_socket, &m_connectex))
					goto _ERROR;
			}
			LPOVERLAPPED ps = static_cast<LPOVERLAPPED>(context);
			if (!m_connectex(m_socket, (SOCKADDR*)&si, sizeof(si), NULL, 0, NULL, ps) &&
				ERROR_IO_PENDING != WSAGetLastError())
			{
				goto _ERROR;
			}
			return TRUE;
		_ERROR:
			errorCode = WSAGetLastError();
			if (!callback.IsNull())
				callback(errorCode, 0, &context->Address, arg);
			SAFE_DELETE(context);
			Close();
			return FALSE;
		}
		BOOL TinySocket::BeginReceive(CHAR* data, DWORD dwSize, DWORD dwFlags, CompleteCallback& callback, LPVOID arg)
		{
			ASSERT(m_socket);
			DWORD errorCode = 0;
			if (!m_connect)
				goto _ERROR;
			PER_IO_CONTEXT* context = new PER_IO_CONTEXT();
			ZeroMemory(context, sizeof(PER_IO_CONTEXT));
			context->OP = OP_RECV;
			context->AsyncState = arg;
			context->Reserve = reinterpret_cast<LONG_PTR>(this);
			context->Complete = callback;
			context->Element.buf = data;
			context->Element.len = dwSize;
			LPOVERLAPPED ps = static_cast<LPOVERLAPPED>(context);
			DWORD numberOfBytesRecvd = 0;
			if (WSARecv(m_socket, &context->Element, 1, &numberOfBytesRecvd, &dwFlags, ps, NULL) == SOCKET_ERROR &&
				ERROR_IO_PENDING != WSAGetLastError())
				goto _ERROR;
			return TRUE;
		_ERROR:
			errorCode = WSAGetLastError();
			if (!callback.IsNull())
				callback(errorCode, 0, &context->Address, arg);
			SAFE_DELETE(context);
			Close();
			return FALSE;
		}
		BOOL TinySocket::BeginSend(CHAR* data, DWORD dwSize, DWORD dwFlags, CompleteCallback& callback, LPVOID arg)
		{
			ASSERT(m_socket);
			DWORD errorCode = 0;
			if (!m_connect)
				goto _ERROR;
			PER_IO_CONTEXT* context = new PER_IO_CONTEXT();
			ZeroMemory(context, sizeof(PER_IO_CONTEXT));
			context->OP = OP_SEND;
			context->AsyncState = arg;
			context->Reserve = reinterpret_cast<LONG_PTR>(this);
			context->Complete = callback;
			context->Element.buf = data;
			context->Element.len = dwSize;
			LPOVERLAPPED ps = static_cast<LPOVERLAPPED>(context);
			DWORD numberOfBytesRecvd = 0;
			if (WSASend(m_socket, &context->Element, 1, &numberOfBytesRecvd, dwFlags, ps, NULL) == SOCKET_ERROR && ERROR_IO_PENDING != WSAGetLastError())
				goto _ERROR;
			return TRUE;
		_ERROR:
			errorCode = WSAGetLastError();
			if (!callback.IsNull())
				callback(errorCode, 0, &context->Address, arg);
			SAFE_DELETE(context);
			Close();
			return FALSE;
		}
		BOOL TinySocket::BeginSendTo(CHAR* data, DWORD dwSize, DWORD dwFlags, const IPAddress& address, DWORD dwPORT, CompleteCallback& callback, LPVOID arg)
		{
			ASSERT(m_socket);
			DWORD errorCode = 0;
			if (!m_connect)
				goto _ERROR;
			PER_IO_CONTEXT* context = new PER_IO_CONTEXT();
			ZeroMemory(context, sizeof(PER_IO_CONTEXT));
			context->OP = OP_SENDTO;
			context->AsyncState = arg;
			context->Reserve = reinterpret_cast<LONG_PTR>(this);
			context->Complete = callback;
			context->Element.buf = data;
			context->Element.len = dwSize;
			ZeroMemory(&context->Address, sizeof(context->Address));
			context->Address.sin_family = m_addressFamily;
			context->Address.sin_port = htons(static_cast<USHORT>(dwPORT));
			memcpy(&context->Address.sin_addr, address.Address().data(), m_addressFamily == AF_INET ? IPAddress::IPv4AddressSize : IPAddress::IPv6AddressSize);
			LPOVERLAPPED ps = static_cast<LPOVERLAPPED>(context);
			DWORD dwBytes = 0;
			if (WSASendTo(m_socket, &context->Element, 1, &dwBytes, dwFlags, (SOCKADDR*)&context->Address, sizeof(context->Address), ps, NULL) == SOCKET_ERROR &&
				ERROR_IO_PENDING != WSAGetLastError())
				goto _ERROR;
			return TRUE;
		_ERROR:
			errorCode = WSAGetLastError();
			if (!callback.IsNull())
				callback(errorCode, 0, &context->Address, arg);
			SAFE_DELETE(context);
			Close();
			return FALSE;
		}
		BOOL TinySocket::BeginReceiveFrom(CHAR* data, DWORD dwSize, DWORD dwFlags, CompleteCallback& callback, LPVOID arg)
		{
			ASSERT(m_socket);
			DWORD errorCode = 0;
			if (!m_connect)
				goto _ERROR;
			PER_IO_CONTEXT* context = new PER_IO_CONTEXT();
			ZeroMemory(context, sizeof(PER_IO_CONTEXT));
			context->OP = OP_RECVFROM;
			context->AsyncState = arg;
			context->Reserve = reinterpret_cast<LONG_PTR>(this);
			context->Complete = callback;
			context->Element.buf = data;
			context->Element.len = dwSize;
			ZeroMemory(&context->Address, sizeof(context->Address));
			context->Address.sin_family = m_addressFamily;
			context->Address.sin_port = 0;
			context->Address.sin_addr.s_addr = htonl(INADDR_ANY);
			LPOVERLAPPED ps = static_cast<LPOVERLAPPED>(context);
			DWORD dwBytes = 0;
			INT size = 0;
			if (WSARecvFrom(m_socket, &context->Element, 1, &dwBytes, &dwFlags, (SOCKADDR*)&context->Address, &size, ps, NULL) == SOCKET_ERROR &&
				ERROR_IO_PENDING != WSAGetLastError())
				goto _ERROR;
			return TRUE;
		_ERROR:
			errorCode = WSAGetLastError();
			if (!callback.IsNull())
				callback(errorCode, 0, &context->Address, arg);
			SAFE_DELETE(context);
			Close();
			return FALSE;
		}
		BOOL TinySocket::BeginDisconnect(CompleteCallback& callback, LPVOID arg)
		{
			DWORD errorCode = 0;
			if (!m_disconnectex)
			{
				if (!TinySocket::GetDisconnectEx(m_socket, &m_disconnectex))
					goto _ERROR;
			}
			PER_IO_CONTEXT* context = new PER_IO_CONTEXT();
			ZeroMemory(context, sizeof(PER_IO_CONTEXT));
			context->OP = OP_DISCONNECT;
			context->AsyncState = arg;
			context->Reserve = reinterpret_cast<LONG_PTR>(this);
			context->Complete = callback;
			LPOVERLAPPED ps = static_cast<LPOVERLAPPED>(context);
			if (!m_disconnectex(m_socket, ps, TF_REUSE_SOCKET, 0) &&
				ERROR_IO_PENDING != WSAGetLastError())
			{
				goto _ERROR;
			}
		_ERROR:
			errorCode = WSAGetLastError();
			if (!callback.IsNull())
				callback(errorCode, 0, &context->Address, arg);
			SAFE_DELETE(context);
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
				m_disconnectex = NULL;
				m_acceptex = NULL;
				m_connectex = NULL;
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