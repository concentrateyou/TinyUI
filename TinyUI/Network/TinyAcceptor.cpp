#include "../stdafx.h"
#include "TinyAcceptor.h"
#include <process.h>

namespace TinyUI
{
	namespace Network
	{
		TinyAcceptor::TinyAcceptor(SOCKET listen)
			:m_listen(listen)
		{

		}

		TinyAcceptor::~TinyAcceptor()
		{

		}

		BOOL TinyAcceptor::BeginAccept()
		{
			SOCKET socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			LPFN_ACCEPTEX acceptex = NULL;
			if (TinySocket::GetAcceptEx(socket, &acceptex))
			{
				ACCEPT_IO_CONTEXT* context = new ACCEPT_IO_CONTEXT();
				context->OP = OP_ACCEPT;
				context->socket = socket;
				context->callback = BindCallback(&TinyAcceptor::IOCompletion, this);
				DWORD dwBytes = 0;
				return acceptex(m_listen, socket, m_data, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwBytes, context);
			}
			return FALSE;
		}

		void TinyAcceptor::IOCompletion(DWORD dwCode, DWORD dwBytes, IO_CONTEXT*ps)
		{
			if (dwCode == 0)
			{
				if (dwBytes > 0)
				{
					//第一个数据块
					ACCEPT_IO_CONTEXT* s = static_cast<ACCEPT_IO_CONTEXT*>(ps);
					LPFN_GETACCEPTEXSOCKADDRS getAcceptExSockaddrs = NULL;
					if (TinySocket::GetAcceptExSockaddrs(s->socket, &getAcceptExSockaddrs))
					{

					}
				}
				//继续异步调用
				this->BeginAccept();
			}
		}
	}
}