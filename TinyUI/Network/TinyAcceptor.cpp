#include "../stdafx.h"
#include "TinyAcceptor.h"
#include <process.h>

namespace TinyUI
{
	namespace Network
	{
		TinyAcceptor::TinyAcceptor(SOCKET listenSocket)
			:m_listenSocket(listenSocket)
		{

		}

		TinyAcceptor::~TinyAcceptor()
		{

		}

		BOOL TinyAcceptor::BeginAccept(ULONG_PTR completionKey)
		{
			SOCKET acceptSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			if (acceptSocket == INVALID_SOCKET)
				return FALSE;
			LPFN_ACCEPTEX acceptex = NULL;
			if (TinySocket::GetAcceptEx(m_listenSocket, &acceptex))
			{
				ACCEPT_IO_CONTEXT* context = new ACCEPT_IO_CONTEXT();
				ZeroMemory(context, sizeof(ACCEPT_IO_CONTEXT));
				context->OP = OP_ACCEPT;
				context->ListenSocket = m_listenSocket;
				context->AcceptSocket = acceptSocket;
				context->CompletionKey = completionKey;
				context->Callback = BindCallback(&TinyAcceptor::IOCompletion, this);
				DWORD dwBytes = 0;
				DWORD dwAddressSize = sizeof(SOCKADDR_IN) + 16;
				CHAR data[(sizeof(SOCKADDR_IN) + 16) * 2];
				ZeroMemory(data, (sizeof(SOCKADDR_IN) + 16) * 2);
				LPOVERLAPPED ps = static_cast<LPOVERLAPPED>(context);
				if (!acceptex(m_listenSocket, acceptSocket, data, 0, dwAddressSize, dwAddressSize, &dwBytes, ps))
				{
					if (WSAGetLastError() != ERROR_IO_PENDING)
					{
						closesocket(acceptSocket);
						SAFE_DELETE(context);
						return FALSE;
					}
				}
				return TRUE;
			}
			return FALSE;
		}

		void TinyAcceptor::IOCompletion(DWORD dwCode, DWORD dwBytes, IO_CONTEXT*ps)
		{
			if (dwCode == 0)
			{
				if (dwBytes > 0)
				{
					ACCEPT_IO_CONTEXT* s = static_cast<ACCEPT_IO_CONTEXT*>(ps);
					LPFN_GETACCEPTEXSOCKADDRS getAcceptExSockaddrs = NULL;
					if (TinySocket::GetAcceptExSockaddrs(s->AcceptSocket, &getAcceptExSockaddrs))
					{

					}
				}
				//继续异步调用
				this->BeginAccept(ps->CompletionKey);
			}
		}
	}
}