#include "../stdafx.h"
#include "TinyTCPServer.h"
#include <process.h>

namespace TinyUI
{
	namespace Network
	{
		TinyTCPServer::TinyTCPServer(TinyIOServer* ioserver)
			:m_ioserver(ioserver)
		{

		}
		TinyTCPServer::~TinyTCPServer()
		{

		}
		BOOL TinyTCPServer::Initialize(DWORD dwPORT)
		{
			if (!Attach(WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)))
				return FALSE;
			if (!m_ioserver->GetIOCP()->Register(reinterpret_cast<HANDLE>(m_socket), m_socket))
				return FALSE;
			SOCKADDR_IN si;
			ZeroMemory(&si, sizeof(si));
			si.sin_family = AF_INET;
			si.sin_port = htons(static_cast<USHORT>(dwPORT));
			si.sin_addr.s_addr = htonl(INADDR_ANY);
			if (bind(m_socket, (sockaddr*)&si, sizeof(si)) == SOCKET_ERROR)
				return FALSE;
			if (listen(m_socket, SOMAXCONN) != SOCKET_ERROR)
				return FALSE;
			return TRUE;
		}

		BOOL TinyTCPServer::BeginAccept(CompleteCallback& callback)
		{
			SOCKET socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			if (socket == INVALID_SOCKET)
				return FALSE;
			TinySocket* s = new TinySocket();
			if (!s->Attach(socket))
				goto _ERROR;
			LPFN_ACCEPTEX acceptex = NULL;
			if (!TinySocket::GetAcceptEx(m_socket, &acceptex))
				goto _ERROR;
			ZeroMemory(&s->m_context, sizeof(PER_IO_CONTEXT));
			s->m_context.OP = OP_ACCEPT;
			s->m_context.Key = reinterpret_cast<LONG_PTR>(s);
			DWORD dwBytes = 0;
			DWORD dwAddressSize = sizeof(SOCKADDR_IN) + 16;
			CHAR data[(sizeof(SOCKADDR_IN) + 16) * 2];
			ZeroMemory(data, (sizeof(SOCKADDR_IN) + 16) * 2);
			LPOVERLAPPED ps = static_cast<LPOVERLAPPED>(&s->m_context);
			if (!acceptex(m_socket, socket, data, 0, dwAddressSize, dwAddressSize, &dwBytes, ps) &&
				ERROR_IO_PENDING != WSAGetLastError())
			{
				goto _ERROR;
			}
			return TRUE;
		_ERROR:
			SAFE_DELETE(s);
			return FALSE;
		}
	}
}