#include "../stdafx.h"
#include "TinySocket.h"
#include "TinyIOServer.h"

namespace TinyUI
{
	namespace Network
	{
		TinyPointerMap TinyHandleSOCKET::m_socketMap;
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
			TinyPointerMap& map = TinyHandleSOCKET::m_socketMap;
			map.Add((UINT_PTR)socket, (UINT_PTR)this);
			return TRUE;
		}
		SOCKET TinyHandleSOCKET::Detach()
		{
			SOCKET socket = m_socket;
			if (socket != INVALID_SOCKET)
			{
				TinyPointerMap& map = TinyHandleSOCKET::m_socketMap;
				map.Remove((UINT_PTR)socket);
			}
			m_socket = NULL;
			return socket;
		}
		TinyHandleSOCKET* TinyHandleSOCKET::Lookup(SOCKET socket)
		{
			TinyPointerMap& map = TinyHandleSOCKET::m_socketMap;
			UINT_PTR val = 0;
			if (!map.Lookup((UINT_PTR)socket, val))
				return NULL;
			return reinterpret_cast<TinyHandleSOCKET*>(val);
		}
		BOOL TinyHandleSOCKET::SetOption(INT level, INT opt, const CHAR* optval, INT size)
		{
			ASSERT(m_socket);
			return setsockopt(m_socket, level, opt, optval, size) == S_OK;
		}
		BOOL TinyHandleSOCKET::GetOption(INT level, INT opt, CHAR* optval, INT& size)
		{
			ASSERT(m_socket);
			return getsockopt(m_socket, level, opt, optval, &size) == S_OK;
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
			: m_ioserver(ioserver),
			m_addressFamily(AF_INET),
			m_socketType(SOCK_STREAM),
			m_protocolType(IPPROTO_TCP),
			m_connect(FALSE),
			m_disconnectex(NULL),
			m_acceptex(NULL),
			m_connectex(NULL)
		{

		}
		BOOL TinySocket::Open(INT addressFamily, INT socketType, INT protocolType)
		{
			m_addressFamily = addressFamily;
			m_socketType = socketType;
			m_protocolType = protocolType;
			if (m_ioserver)
			{
				m_socket = WSASocket(addressFamily, socketType, protocolType, NULL, 0, WSA_FLAG_OVERLAPPED);
				BOOL allow = TRUE;
				if (!SetOption(SOL_SOCKET, SO_REUSEADDR, (const CHAR*)&allow, sizeof(allow)))
					return FALSE;
				if (!Attach(m_socket))
					return FALSE;
				return m_ioserver->GetIOCP()->Register(reinterpret_cast<HANDLE>(m_socket), m_socket);
			}
			else
			{
				m_socket = WSASocket(addressFamily, socketType, protocolType, NULL, 0, 0);
				BOOL allow = TRUE;
				if (!SetOption(SOL_SOCKET, SO_REUSEADDR, (const CHAR*)&allow, sizeof(allow)))
					return FALSE;
				if (!Attach(m_socket))
					return FALSE;
			}
			return TRUE;
		}
		BOOL TinySocket::KeepAlive(BOOL bAllow, INT ms)
		{
			ASSERT(m_socket);
			if (SetOption(SOL_SOCKET, SO_KEEPALIVE, (const CHAR*)bAllow, sizeof(bAllow)))
			{
				return TRUE;
			}
			return FALSE;
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
			if (ioctlsocket(m_socket, FIONREAD, (ULONG*)&argp) != S_OK)
				throw error_code(WSAGetLastError(), system_category());
			return argp;
		}
		BOOL TinySocket::Blocking(BOOL bAllow)
		{
			ASSERT(m_socket);
			return ioctlsocket(m_socket, FIONBIO, (ULONG*)&bAllow) != 0;
		}
		BOOL TinySocket::Duplicate(DWORD processID, WSAPROTOCOL_INFO& s)
		{
			ASSERT(m_socket);
			return WSADuplicateSocket(m_socket, processID, &s) != S_OK;
		}
		TinySocket::~TinySocket()
		{
			Close();
		}
		TinyIOServer* TinySocket::GetIOServer() const
		{
			return m_ioserver;
		}
		BOOL TinySocket::Bind(const IPEndPoint& endpoint)
		{
			ASSERT(m_socket);
			SOCKADDR s = { 0 };
			size_t size = 0;
			if (endpoint.ToSOCKADDR(&s, &size))
			{
				return bind(m_socket, &s, size) == S_OK;
			}
			return FALSE;
		}
		BOOL TinySocket::Listen(DWORD backlog)
		{
			ASSERT(m_socket);
			return listen(m_socket, SOMAXCONN) == S_OK;
		}
		TinySocket* TinySocket::Accept()
		{
			ASSERT(m_socket);
			SOCKADDR_IN sa = { 0 };
			INT size = sizeof(SOCKADDR_IN);
			SOCKET s = accept(m_socket, (SOCKADDR*)&sa, &size);
			if (s != INVALID_SOCKET)
			{
				TinySocket* socket = new TinySocket();
				if (!socket || !socket->Attach(s))
					return NULL;
				socket->m_addressFamily = this->m_addressFamily;
				socket->m_protocolType = this->m_protocolType;
				socket->m_socketType = this->m_socketType;
				socket->m_connect = TRUE;
				return socket;
			}
			return NULL;
		}
		BOOL TinySocket::Connect(const IPEndPoint& endpoint)
		{
			ASSERT(m_socket);
			SOCKADDR si = { 0 };
			size_t size = 0;
			if (endpoint.ToSOCKADDR(&si, &size))
			{
				m_connect = connect(m_socket, &si, size) == S_OK;
				return m_connect;
			}
			return FALSE;
		}
		INT  TinySocket::Receive(CHAR* data, DWORD dwSize, DWORD dwFlag)
		{
			ASSERT(m_socket);
			return recv(m_socket, data, dwSize, dwFlag);
		}
		INT	 TinySocket::Send(CHAR* data, DWORD dwSize, DWORD dwFlag)
		{
			ASSERT(m_socket);
			return send(m_socket, data, dwSize, dwFlag);
		}
		INT	TinySocket::ReceiveFrom(CHAR* data, DWORD dwSize, DWORD dwFlags, IPEndPoint& endpoint)
		{
			ASSERT(m_socket);
			SOCKADDR si = { 0 };
			INT size = 0;
			INT iRes = recvfrom(m_socket, data, dwSize, dwFlags, (SOCKADDR*)&si, &size);
			endpoint.FromSOCKADDR(&si, size);
			return iRes;
		}
		INT	 TinySocket::SendTo(CHAR* data, DWORD dwSize, DWORD dwFlag, IPEndPoint& endpoint)
		{
			ASSERT(m_socket);
			SOCKADDR si = { 0 };
			INT size = 0;
			INT iRes = sendto(m_socket, data, dwSize, dwFlag, (SOCKADDR*)&si, sizeof(si));
			endpoint.FromSOCKADDR(&si, size);
			return iRes;
		}
		BOOL TinySocket::Post(CompleteCallback&& callback, AsyncResult* result, LPVOID arg)
		{
			ASSERT(m_ioserver);
			PER_IO_CONTEXT* context = new PER_IO_CONTEXT();
			ZeroMemory(context, sizeof(PER_IO_CONTEXT));
			context->OP = OP_CONNECT;
			context->Reserve = reinterpret_cast<LONG_PTR>(this);
			context->Result = result;
			context->Result->AsyncState = arg;
			context->Complete = std::forward<CompleteCallback>(callback);
			LPOVERLAPPED ps = static_cast<LPOVERLAPPED>(context);
			return PostQueuedCompletionStatus(m_ioserver->GetIOCP(), 0, 0, ps);
		}
		//////////////////////////////////////////////////////////////////////////
		BOOL TinySocket::BeginAccept(CompleteCallback&& callback, LPVOID arg)
		{
			ASSERT(m_ioserver);
			TinyScopedArray<CHAR> data;
			DWORD errorCode = ERROR_SUCCESS;
			TinyAutoLock lock(m_synclock);
			TinySocket* socket = new TinySocket(m_ioserver);
			if (!socket || !socket->Open(m_addressFamily, m_socketType, m_protocolType))
				goto _ERROR;
			if (!m_acceptex)
			{
				if (!TinySocket::GetAcceptEx(m_socket, &m_acceptex))
					goto _ERROR;
			}
			PER_IO_CONTEXT* context = new PER_IO_CONTEXT();
			ZeroMemory(context, sizeof(PER_IO_CONTEXT));
			context->OP = OP_ACCEPT;
			context->Reserve = reinterpret_cast<LONG_PTR>(socket);
			context->Result = new AcceptAsyncResult();
			context->Result->AsyncState = arg;
			context->Complete = std::forward<CompleteCallback>(callback);
			DWORD dwBytes = 0;
			DWORD dwAddressSize = m_addressFamily == AF_INET ? sizeof(SOCKADDR_IN) + 16 : sizeof(SOCKADDR_IN6) + 16;
			data.Reset(new CHAR[dwAddressSize * 2]);
			ZeroMemory(data, dwAddressSize * 2);
			LPOVERLAPPED ps = static_cast<LPOVERLAPPED>(context);
			if (!m_acceptex(m_socket, socket->Handle(), data, 0, dwAddressSize, dwAddressSize, &dwBytes, ps) &&
				ERROR_IO_PENDING != WSAGetLastError())
			{
				goto _ERROR;
			}
			return TRUE;
		_ERROR:
			errorCode = WSAGetLastError();
			if (!callback.IsNull())
			{
				callback(errorCode, context->Result);
			}
			SAFE_DELETE(context);
			SAFE_DELETE(socket);
			return FALSE;
		}
		TinySocket* TinySocket::EndAccept(AsyncResult* result)
		{
			AcceptAsyncResult* s = static_cast<AcceptAsyncResult*>(result);
			ASSERT(s);
			return s->AcceptSocket;
		}
		BOOL TinySocket::BeginConnect(const IPEndPoint& endpoint, CompleteCallback&& callback, LPVOID arg)
		{
			//https://msdn.microsoft.com/en-us/library/windows/desktop/ms737606(v=vs.85).aspx
			ASSERT(m_ioserver);
			TinyAutoLock lock(m_synclock);
			DWORD errorCode = ERROR_SUCCESS;
			SOCKADDR s = { 0 };
			s.sa_family = m_addressFamily;
			if (bind(m_socket, (SOCKADDR*)&s, m_addressFamily == AF_INET ? IPAddress::IPv4AddressSize : IPAddress::IPv6AddressSize) != S_OK)
				goto _ERROR;
			PER_IO_CONTEXT* context = new PER_IO_CONTEXT();
			ZeroMemory(context, sizeof(PER_IO_CONTEXT));
			context->OP = OP_CONNECT;
			context->Reserve = reinterpret_cast<LONG_PTR>(this);
			context->Result = new AsyncResult();
			context->Result->AsyncState = arg;
			context->Complete = std::forward<CompleteCallback>(callback);
			size_t size = 0;
			if (!endpoint.ToSOCKADDR(&s, &size))
			{
				WSASetLastError(ERROR_INVALID_ADDRESS);
				goto _ERROR;
			}
			if (!m_connectex)
			{
				if (!TinySocket::GetConnectEx(m_socket, &m_connectex))
					goto _ERROR;
			}
			LPOVERLAPPED ps = static_cast<LPOVERLAPPED>(context);
			if (!m_connectex(m_socket, &s, size, NULL, 0, NULL, ps) &&
				ERROR_IO_PENDING != WSAGetLastError())
			{
				goto _ERROR;
			}
			return TRUE;
		_ERROR:
			errorCode = WSAGetLastError();
			if (!callback.IsNull())
			{
				callback(errorCode, context->Result);
			}
			SAFE_DELETE(context);
			Close();
			return FALSE;
		}
		void TinySocket::EndConnect(AsyncResult* result)
		{

		}
		BOOL TinySocket::BeginReceive(CHAR* data, DWORD dwSize, DWORD dwFlags, CompleteCallback&& callback, LPVOID arg)
		{
			ASSERT(m_socket || m_ioserver);
			TinyAutoLock lock(m_synclock);
			DWORD errorCode = ERROR_SUCCESS;
			if (!m_connect)
				goto _ERROR;
			PER_IO_CONTEXT* context = new PER_IO_CONTEXT();
			ZeroMemory(context, sizeof(PER_IO_CONTEXT));
			context->OP = OP_RECV;
			context->Reserve = reinterpret_cast<LONG_PTR>(this);
			StreamAsyncResult* result = new StreamAsyncResult();
			context->Result = result;
			context->Result->AsyncState = arg;
			context->Complete = std::forward<CompleteCallback>(callback);
			result->Array.buf = data;
			result->Array.len = dwSize;
			LPOVERLAPPED ps = static_cast<LPOVERLAPPED>(context);
			DWORD numberOfBytesRecvd = 0;
			if (WSARecv(m_socket, &result->Array, 1, &numberOfBytesRecvd, &dwFlags, ps, NULL) != S_OK &&
				ERROR_IO_PENDING != WSAGetLastError())
				goto _ERROR;
			return TRUE;
		_ERROR:
			errorCode = WSAGetLastError();
			if (!callback.IsNull())
			{
				callback(errorCode, context->Result);
			}
			SAFE_DELETE(context);
			Close();
			return FALSE;
		}
		INT TinySocket::EndReceive(AsyncResult* result)
		{
			StreamAsyncResult* s = static_cast<StreamAsyncResult*>(result);
			return s->BytesTransferred;
		}
		BOOL TinySocket::BeginSend(CHAR* data, DWORD dwSize, DWORD dwFlag, CompleteCallback&& callback, LPVOID arg)
		{
			ASSERT(m_socket || m_ioserver);
			TinyAutoLock lock(m_synclock);
			DWORD errorCode = ERROR_SUCCESS;
			if (!m_connect)
				goto _ERROR;
			PER_IO_CONTEXT* context = new PER_IO_CONTEXT();
			ZeroMemory(context, sizeof(PER_IO_CONTEXT));
			context->OP = OP_SEND;
			context->Reserve = reinterpret_cast<LONG_PTR>(this);
			StreamAsyncResult* result = new StreamAsyncResult();
			context->Result = result;
			context->Result->AsyncState = arg;
			context->Complete = std::forward<CompleteCallback>(callback);
			result->Array.buf = data;
			result->Array.len = dwSize;
			LPOVERLAPPED ps = static_cast<LPOVERLAPPED>(context);
			DWORD numberOfBytesRecvd = 0;
			if (WSASend(m_socket, &result->Array, 1, &numberOfBytesRecvd, dwFlag, ps, NULL) != S_OK &&
				ERROR_IO_PENDING != WSAGetLastError())
			{
				goto _ERROR;
			}
			return TRUE;
		_ERROR:
			errorCode = WSAGetLastError();
			if (!callback.IsNull())
			{
				callback(errorCode, context->Result);
			}
			SAFE_DELETE(context);
			Close();
			return FALSE;
		}
		INT  TinySocket::EndSend(AsyncResult* result)
		{
			StreamAsyncResult* s = static_cast<StreamAsyncResult*>(result);
			return s->BytesTransferred;
		}
		BOOL TinySocket::BeginSendTo(CHAR* data, DWORD dwSize, DWORD dwFlags, IPEndPoint& endpoint, CompleteCallback&& callback, LPVOID arg)
		{
			ASSERT(m_socket || m_ioserver);
			TinyAutoLock lock(m_synclock);
			DWORD errorCode = ERROR_SUCCESS;
			SOCKADDR s = { 0 };
			size_t size = 0;
			if (!endpoint.ToSOCKADDR(&s, &size))
			{
				WSASetLastError(ERROR_INVALID_ADDRESS);
				goto _ERROR;
			}
			PER_IO_CONTEXT* context = new PER_IO_CONTEXT();
			ZeroMemory(context, sizeof(PER_IO_CONTEXT));
			context->OP = OP_SENDTO;
			context->Reserve = reinterpret_cast<LONG_PTR>(this);
			context->Complete = std::forward<CompleteCallback>(callback);
			DatagramAsyncResult* result = new DatagramAsyncResult();
			context->Result = result;
			result->AsyncState = arg;
			result->Array.buf = data;
			result->Array.len = dwSize;
			result->Address = s;
			result->Size = size;
			LPOVERLAPPED ps = static_cast<LPOVERLAPPED>(context);
			DWORD dwBytes = 0;
			if (WSASendTo(m_socket, &result->Array, 1, &dwBytes, dwFlags, &result->Address, size, ps, NULL) != S_OK &&
				ERROR_IO_PENDING != WSAGetLastError())
			{
				goto _ERROR;
			}
			return TRUE;
		_ERROR:
			errorCode = WSAGetLastError();
			if (!callback.IsNull())
			{
				callback(errorCode, context->Result);
			}
			SAFE_DELETE(context);
			Close();
			return FALSE;
		}
		INT  TinySocket::EndSendTo(AsyncResult* result)
		{
			DatagramAsyncResult* s = static_cast<DatagramAsyncResult*>(result);
			return s->BytesTransferred;
		}
		BOOL TinySocket::BeginReceiveFrom(CHAR* data, DWORD dwSize, DWORD dwFlags, CompleteCallback&& callback, LPVOID arg)
		{
			ASSERT(m_socket || m_ioserver);
			TinyAutoLock lock(m_synclock);
			DWORD errorCode = ERROR_SUCCESS;
			PER_IO_CONTEXT* context = new PER_IO_CONTEXT();
			ZeroMemory(context, sizeof(PER_IO_CONTEXT));
			context->OP = OP_RECVFROM;
			context->Reserve = reinterpret_cast<LONG_PTR>(this);
			context->Complete = std::forward<CompleteCallback>(callback);
			DatagramAsyncResult* result = new DatagramAsyncResult();
			context->Result = result;
			result->Array.buf = data;
			result->Array.len = dwSize;
			ZeroMemory(&result->Address, sizeof(SOCKADDR));
			result->Address.sa_family = m_addressFamily;
			LPOVERLAPPED ps = static_cast<LPOVERLAPPED>(context);
			DWORD dwBytes = 0;
			INT size = m_addressFamily == AF_INET ? IPAddress::IPv4AddressSize : IPAddress::IPv6AddressSize;
			if (WSARecvFrom(m_socket, &result->Array, 1, &dwBytes, &dwFlags, (SOCKADDR*)&result->Address, &size, ps, NULL) != S_OK &&
				ERROR_IO_PENDING != WSAGetLastError())
			{
				goto _ERROR;
			}
			return TRUE;
		_ERROR:
			errorCode = WSAGetLastError();
			if (!callback.IsNull())
			{
				callback(errorCode, context->Result);
			}
			SAFE_DELETE(context);
			Close();
			return FALSE;
		}
		INT  TinySocket::EndReceiveFrom(AsyncResult* result, IPEndPoint& endpoint)
		{
			DatagramAsyncResult* s = static_cast<DatagramAsyncResult*>(result);
			ASSERT(s);
			endpoint.FromSOCKADDR(&s->Address, s->Size);
			return s->BytesTransferred;
		}
		BOOL TinySocket::BeginDisconnect(CompleteCallback&& callback, LPVOID arg)
		{
			ASSERT(m_socket || m_ioserver);
			TinyAutoLock lock(m_synclock);
			DWORD errorCode = ERROR_SUCCESS;
			if (!m_disconnectex)
			{
				if (!TinySocket::GetDisconnectEx(m_socket, &m_disconnectex))
					goto _ERROR;
			}
			PER_IO_CONTEXT* context = new PER_IO_CONTEXT();
			ZeroMemory(context, sizeof(PER_IO_CONTEXT));
			context->OP = OP_DISCONNECT;
			context->Reserve = reinterpret_cast<LONG_PTR>(this);
			context->Result = new AsyncResult();
			context->Result->AsyncState = arg;
			context->Complete = std::forward<CompleteCallback>(callback);
			LPOVERLAPPED ps = static_cast<LPOVERLAPPED>(context);
			if (!m_disconnectex(m_socket, ps, TF_REUSE_SOCKET, 0) &&
				ERROR_IO_PENDING != WSAGetLastError())
			{
				goto _ERROR;
			}
		_ERROR:
			errorCode = WSAGetLastError();
			if (!callback.IsNull())
			{
				callback(errorCode, context->Result);
			}
			SAFE_DELETE(context);
			Close();
			return FALSE;
		}
		void TinySocket::EndDisconnect(AsyncResult* result)
		{

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
				if (shutdown(m_socket, how) == S_OK)
				{
					m_disconnectex = NULL;
					m_acceptex = NULL;
					m_connectex = NULL;
					return TRUE;
				}
			}
			return FALSE;
		}
	}
}