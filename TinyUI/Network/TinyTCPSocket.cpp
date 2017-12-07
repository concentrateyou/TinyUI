#include "../stdafx.h"
#include "TinyTCPSocket.h"

namespace TinyUI
{
	namespace Network
	{
		TinyTCPSocket::TinyTCPSocket()
			:m_handle(NULL)
		{
			ZeroMemory(&m_readBuffer, sizeof(m_readBuffer));
			ZeroMemory(&m_writeBuffer, sizeof(m_writeBuffer));
			ZeroMemory(&m_readIO, sizeof(m_readIO));
			m_readIO.hEvent = WSACreateEvent();
			ZeroMemory(&m_writeIO, sizeof(m_writeIO));
			m_writeIO.hEvent = WSACreateEvent();
		}
		TinyTCPSocket::~TinyTCPSocket()
		{
			if (m_readIO.hEvent != NULL)
			{
				WSACloseEvent(m_readIO.hEvent);
				m_readIO.hEvent = NULL;
			}
			if (m_writeIO.hEvent != NULL)
			{
				WSACloseEvent(m_writeIO.hEvent);
				m_writeIO.hEvent = NULL;
			}
			this->Close();
		}
		BOOL TinyTCPSocket::Open(AddressFamily addressFamily)
		{
			INT family = ConvertAddressFamily(addressFamily);
			if (!this->Attach(::WSASocket(family, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED)))
				return FALSE;
			if (family == AF_INET6)
			{
				DWORD value = 0;
				if (::setsockopt(m_socket, IPPROTO_IPV6, IPV6_V6ONLY, reinterpret_cast<const char*>(&value), sizeof(value)) != 0)
				{
					OnError(WSAGetLastError());
					goto _ERROR;
				}
			}
			ULONG iMode = 1;
			if (::ioctlsocket(m_socket, FIONBIO, &iMode) != 0)
			{
				OnError(WSAGetLastError());
				goto _ERROR;
			}
			return TRUE;
		_ERROR:
			this->Close();
			return FALSE;
		}
		BOOL TinyTCPSocket::Close()
		{
			if (m_handle != NULL)
			{
				UnregisterWait(m_handle);
				m_handle = NULL;
			}
			SOCKET socket = this->Detach();
			if (socket != INVALID_SOCKET && socket != NULL)
			{
				::shutdown(socket, SD_SEND);
				::closesocket(socket);
				socket = NULL;
			}
			return TRUE;
		}
		BOOL TinyTCPSocket::IsEmpty() const
		{
			return (m_socket == INVALID_SOCKET || m_socket == NULL);
		}
		void CALLBACK TinyTCPSocket::WaitOrTimerCallback(void* ps, BOOLEAN time)
		{
			Context* pThis = reinterpret_cast<Context*>(ps);
			if (pThis != NULL)
			{
				if (pThis->m_op == OP_CONNECT)
				{
					pThis->m_this->ConnectCallback(pThis, time);
				}
				if (pThis->m_op == OP_RECV)
				{
					pThis->m_this->ReceiveCallback(pThis);
				}
			}
			SAFE_DELETE(pThis);
		}
		BOOL TinyTCPSocket::ConnectAsync(const IPEndPoint& endpoint, DWORD dwMS, CompletionCallback&& callback)
		{
			INT iRes = 0;
			SOCKADDR si = { 0 };
			size_t size = 0;
			if (!endpoint.ToSOCKADDR(&si, &size))
			{
				goto _ERROR;
			}
			if (m_socket != INVALID_SOCKET && m_socket != NULL)
			{
				if (SetAsyncEventSelect(OP_CONNECT, FD_CONNECT, dwMS, m_readIO.hEvent, std::move(callback)))
				{
					iRes = WSAConnect(m_socket, &si, size, NULL, NULL, NULL, NULL);
					if (iRes != NO_ERROR)
					{
						iRes = WSAGetLastError();
						if (iRes != WSAEWOULDBLOCK)
						{
							OnError(iRes);
							goto _ERROR;
						}
					}
				}
				return TRUE;
			}
		_ERROR:
			UnsetAsyncEventSelect(m_readIO.hEvent);
			return FALSE;
		}
		void TinyTCPSocket::ConnectCallback(Context* context, BOOL time)
		{
			if (context != NULL &&
				m_socket != NULL &&
				m_socket != INVALID_SOCKET)
			{
				WSANETWORKEVENTS events = { 0 };
				INT iError = 0;
				INT iRes = WSAEnumNetworkEvents(m_socket, m_readIO.hEvent, &events);
				if (iRes == SOCKET_ERROR)
				{
					iError = WSAGetLastError();
					OnError(iError);
					goto _ERROR;
				}
				else
				{
					iError = events.iErrorCode[FD_CONNECT_BIT];
					if (iError != 0)
					{
						OnError(iError);
						goto _ERROR;
					}
				}
				context->m_callback(time, iError);
			}
		_ERROR:
			UnsetAsyncEventSelect(m_readIO.hEvent);
		}
		void TinyTCPSocket::ReceiveCallback(Context* context)
		{
			if (context != NULL &&
				m_socket != NULL &&
				m_socket != INVALID_SOCKET)
			{
				WSANETWORKEVENTS events = { 0 };
				INT iError = 0;
				INT iRes = WSAEnumNetworkEvents(m_socket, m_readIO.hEvent, &events);
				if (iRes == SOCKET_ERROR)
				{
					iError = WSAGetLastError();
					OnError(iError);
					goto _ERROR;
				}
				else
				{
					iError = events.iErrorCode[FD_READ_BIT];
					if (iError != 0)
					{
						OnError(iError);
						goto _ERROR;
					}
				}
				context->m_callback(0, iError);
			}
		_ERROR:
			UnsetAsyncEventSelect(m_readIO.hEvent);
		}
		BOOL TinyTCPSocket::SetAsyncEventSelect(INT op, INT bits, DWORD dwMS, HANDLE hEvent, CompletionCallback&& callback)
		{
			INT iRes = 0;
			Context* connext = new Context();
			connext->m_callback = std::move(callback);
			connext->m_op = op;
			connext->m_this = this;
			if (!RegisterWaitForSingleObject(&m_handle, hEvent, TinyTCPSocket::WaitOrTimerCallback, static_cast<LPVOID>(connext), dwMS, WT_EXECUTEINWAITTHREAD | WT_EXECUTEONLYONCE))
			{
				iRes = GetLastError();
				OnError(iRes);
				goto _ERROR;
			}
			iRes = WSAEventSelect(m_socket, hEvent, bits);
			if (iRes != NO_ERROR)
			{
				iRes = WSAGetLastError();
				OnError(iRes);
				goto _ERROR;
			}
			return TRUE;
		_ERROR:
			UnsetAsyncEventSelect(hEvent);
			return FALSE;
		}
		void TinyTCPSocket::UnsetAsyncEventSelect(HANDLE hEvent)
		{
			if (m_handle != NULL)
			{
				UnregisterWait(m_handle);
				m_handle = NULL;
			}
			WSAEventSelect(m_socket, NULL, 0);
			if (hEvent != NULL)
			{
				WSAResetEvent(hEvent);
			}
		}
		BOOL TinyTCPSocket::ReceiveAsync(CHAR* data, DWORD dwSize, DWORD dwFlags, CompletionCallback&& callback)
		{
			INT iRes = 0;
			if (m_socket != INVALID_SOCKET && m_socket != NULL)
			{
				if (SetAsyncEventSelect(OP_RECV, FD_READ | FD_CLOSE, INFINITE, m_readIO.hEvent, std::move(callback)))
				{
					m_readBuffer.buf = data;
					m_readBuffer.len = static_cast<ULONG>(dwSize);
					iRes = WSARecv(m_socket, &m_readBuffer, 1, NULL, &dwFlags, &m_readIO, NULL);
					if (iRes != NO_ERROR)
					{
						iRes = WSAGetLastError();
						if (iRes != WSAEWOULDBLOCK)
						{
							OnError(iRes);
							goto _ERROR;
						}
					}
				}
				return TRUE;
			}
		_ERROR:
			UnsetAsyncEventSelect(m_readIO.hEvent);
			return FALSE;
		}
		void TinyTCPSocket::OnError(INT iError)
		{
			//TODO;
		}
	}
}