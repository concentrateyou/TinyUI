#include "../stdafx.h"
#include "TinyTCPSocket.h"

namespace TinyUI
{
	namespace Network
	{
		TinyTCPSocket::TinyTCPSocket()
			:m_handle(NULL)
		{
			ZeroMemory(&m_readIO, sizeof(m_readIO));
			ZeroMemory(&m_writeIO, sizeof(m_writeIO));
		}
		TinyTCPSocket::~TinyTCPSocket()
		{

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
			m_readIO.hEvent = WSACreateEvent();
			m_writeIO.hEvent = WSACreateEvent();
			return TRUE;
		_ERROR:
			this->Close();
			return FALSE;
		}
		BOOL TinyTCPSocket::Close()
		{
			SOCKET socket = this->Detach();
			if (socket != INVALID_SOCKET &&
				socket != NULL)
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
				WSAEventSelect(m_socket, m_readIO.hEvent, FD_CONNECT);
				iRes = ::connect(m_socket, &si, size);
				if (iRes == SOCKET_ERROR)
				{
					iRes == WSAGetLastError();
					if (iRes != WSAEWOULDBLOCK)
					{
						OnError(iRes);
						goto _ERROR;
					}
				}
				if (!RegisterWaitForSingleObject(&m_handle, m_readIO.hEvent, TinyTCPSocket::WaitOrTimeCallback, static_cast<LPVOID>(this), dwMS, WT_EXECUTEINWAITTHREAD | WT_EXECUTEONLYONCE))
				{
					goto _ERROR;
				}
				return TRUE;
			}
		_ERROR:
			if (m_handle != NULL)
			{
				UnregisterWait(m_handle);
				m_handle = NULL;
			}
			return FALSE;
		}
		void CALLBACK TinyTCPSocket::WaitOrTimeCallback(void* ps, BOOLEAN time)
		{
			TinyTCPSocket* tcp = reinterpret_cast<TinyTCPSocket*>(ps);
			if (tcp != NULL)
			{

			}
		}
		void TinyTCPSocket::OnError(INT iError)
		{
			//TODO;
		}
	}
}