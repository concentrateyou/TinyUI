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
			SOCKET socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
			LPFN_ACCEPTEX acceptex = NULL;
			if (TinySocket::GetAcceptEx(socket, &acceptex))
			{
				ACCEPT_IO_CONTEXT* context = new ACCEPT_IO_CONTEXT();
				ZeroMemory(context, sizeof(ACCEPT_IO_CONTEXT));
				context->OP = OP_ACCEPT;
				context->Socket = socket;
				//context->callback = BindCallback(&TinyAcceptor::IOCompletion, this);
				DWORD dwBytes = 0;
				DWORD dwAddressSize = sizeof(SOCKADDR_IN) + 16;
				CHAR data[(sizeof(SOCKADDR_IN) + 16) * 2];
				ZeroMemory(data, (sizeof(SOCKADDR_IN) + 16) * 2);
				BOOL bRes = acceptex(m_listen, socket, data, 0, dwAddressSize, dwAddressSize, &dwBytes, &(context->OverLapped));
				DWORD dwError = WSAGetLastError();
				return bRes;
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
					//ACCEPT_IO_CONTEXT* s = static_cast<ACCEPT_IO_CONTEXT*>(ps);
					/*LPFN_GETACCEPTEXSOCKADDRS getAcceptExSockaddrs = NULL;
					if (TinySocket::GetAcceptExSockaddrs(s->socket, &getAcceptExSockaddrs))
					{

					}*/
				}
				//继续异步调用
				this->BeginAccept();
			}
		}
	}
}