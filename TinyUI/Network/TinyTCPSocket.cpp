#include "../stdafx.h"
#include "TinyTCPSocket.h"

namespace TinyUI
{
	namespace Network
	{
		TinyTCPSocket::TinyTCPSocket()
			:m_handle(NULL)
		{
			m_event = WSACreateEvent();
		}
		TinyTCPSocket::~TinyTCPSocket()
		{
			if (m_event != NULL)
			{
				WSACloseEvent(m_event);
				m_event = NULL;
			}
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
				if (!time)
				{
					if (pThis->m_currentFD == FD_CONNECT)
					{
						pThis->m_this->ConnectCallback();
					}
				}

			}
		}
		BOOL TinyTCPSocket::Connect(const IPEndPoint& endpoint, DWORD dwMS, CompletionCallback&& callback)
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
				INT iRes = S_OK;
				Context* connext = new Context();
				connext->m_callback = std::move(callback);
				connext->m_currentFD = FD_CONNECT;
				connext->m_this = this;
				if (!RegisterWaitForSingleObject(&m_handle, m_event, TinyTCPSocket::WaitOrTimerCallback, static_cast<LPVOID>(connext), dwMS, WT_EXECUTEINWAITTHREAD | WT_EXECUTEONLYONCE))
				{
					iRes = GetLastError();
					OnError(iRes);
					goto _ERROR;
				}
				iRes = WSAEventSelect(m_socket, m_event, FD_CONNECT);
				if (iRes != NO_ERROR)
				{
					iRes = WSAGetLastError();
					OnError(iRes);
					goto _ERROR;
				}
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
				return TRUE;
			}
		_ERROR:
			UnsetAsyncEventSelect();
			return FALSE;
		}
		void TinyTCPSocket::ConnectCallback()
		{
			if (m_socket != NULL && m_socket != INVALID_SOCKET)
			{
				WSANETWORKEVENTS events = { 0 };
				events.lNetworkEvents = FD_CONNECT;
				INT iError = 0;
				INT iRes = WSAEnumNetworkEvents(m_socket, m_event, &events);
				if (iRes == SOCKET_ERROR)
				{
					iError = WSAGetLastError();
					OnError(iError);
					goto _ERROR;
				}
				else
				{
					iError = events.iErrorCode[FD_CONNECT_BIT];
					OnError(iError);
					goto _ERROR;
				}

			}
		_ERROR:
			UnsetAsyncEventSelect();
		}
		void TinyTCPSocket::SetAsyncEventSelect(INT bits)
		{

		}
		void TinyTCPSocket::UnsetAsyncEventSelect()
		{
			if (m_handle != NULL)
			{
				UnregisterWait(m_handle);
				m_handle = NULL;
			}
			WSAEventSelect(m_socket, NULL, 0);
			if (m_event != NULL)
			{
				WSAResetEvent(m_event);
			}
		}
		void TinyTCPSocket::OnError(INT iError)
		{
			//TODO;
		}
	}
}