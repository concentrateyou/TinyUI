#include "../stdafx.h"
#include "TinyAcceptor.h"
#include <process.h>

namespace TinyUI
{
	namespace Network
	{
		TinyAcceptor::TinyAcceptor(IO::TinyIOCP& iocp, SOCKET socket)
			:TinySocket(socket),
			m_iocp(iocp)
		{

		}
		BOOL TinyAcceptor::Open(SOCKADDR_IN address)
		{
			if (m_socket == INVALID_SOCKET)
			{
				return FALSE;
			}
			if (bind(m_socket, (SOCKADDR *)&address, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
			{
				return FALSE;
			}
			if (listen(m_socket, 5) == SOCKET_ERROR)
			{
				return FALSE;
			}
			return TRUE;
		}
		BOOL TinyAcceptor::BeginAccept()
		{
			if (m_socket == INVALID_SOCKET)
			{
				return FALSE;
			}
			SOCKET socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			if (socket == INVALID_SOCKET)
			{
				return FALSE;
			}
			return TRUE;
		}
	}
}