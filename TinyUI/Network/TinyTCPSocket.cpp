#include "../stdafx.h"
#include "TinyTCPSocket.h"
#include <process.h>

namespace TinyUI
{
	namespace Network
	{
		TinyTCPSocket::TinyTCPSocket(TinyIOServer* ioserver)
			:m_ioserver(ioserver)
		{

		}
		TinyTCPSocket::~TinyTCPSocket()
		{

		}
		BOOL TinyTCPSocket::Bind(IPAddress& address, DWORD dwPORT)
		{
			if (!Attach(WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)))
				return FALSE;
			if (!m_ioserver->GetIOCP()->Register(reinterpret_cast<HANDLE>(m_socket), m_socket))
				return FALSE;
			SOCKADDR_IN si;
			ZeroMemory(&si, sizeof(si));
			si.sin_family = AF_INET;
			si.sin_port = htons(static_cast<USHORT>(dwPORT));
			memcpy(&si.sin_addr, address.Address().data(), IPAddress::IPv4AddressSize);
			return bind(m_socket, (sockaddr*)&si, sizeof(si)) != SOCKET_ERROR;
		}
		BOOL TinyTCPSocket::Listen(INT backlog)
		{
			ASSERT(m_socket);
			return listen(m_socket, backlog) != SOCKET_ERROR;
		}
		BOOL TinyTCPSocket::BeginAccept()
		{
			SOCKET socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			if (socket == INVALID_SOCKET)
				return FALSE;
			TinyTCPSocket* s = new TinyTCPSocket(m_ioserver);
			if (!s->Attach(socket))
			{
				SAFE_DELETE(s);
				return FALSE;
			}
			LPFN_ACCEPTEX acceptex = NULL;
			if (TinySocket::GetAcceptEx(m_socket, &acceptex))
			{
				ZeroMemory(&s->m_context, sizeof(PER_IO_CONTEXT));
				s->m_context.OP = OP_ACCEPT;
				s->m_context.Key = static_cast<LONG_PTR>(socket);
				DWORD dwBytes = 0;
				DWORD dwAddressSize = sizeof(SOCKADDR_IN) + 16;
				CHAR data[(sizeof(SOCKADDR_IN) + 16) * 2];
				ZeroMemory(data, (sizeof(SOCKADDR_IN) + 16) * 2);
				LPOVERLAPPED ps = static_cast<LPOVERLAPPED>(&s->m_context);
				if (!acceptex(m_socket, socket, data, 0, dwAddressSize, dwAddressSize, &dwBytes, ps) &&
					ERROR_IO_PENDING != WSAGetLastError())
				{
					SAFE_DELETE(s);
					return FALSE;
				}
				return TRUE;
			}
		}
		void TinyTCPSocket::OnAccept(DWORD dwCode, DWORD dwNumberOfBytesTransferred, TinySocket* socket)
		{

		}
	}
}