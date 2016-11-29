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
			Close();
		}

		void TinyAcceptor::Close()
		{
			if (m_listenSocket)
			{
				closesocket(m_listenSocket);
				m_listenSocket = NULL;
			}
		}

		BOOL TinyAcceptor::BeginAccept()
		{
			SOCKET acceptSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			if (acceptSocket == INVALID_SOCKET)
				return FALSE;
			LPFN_ACCEPTEX acceptex = NULL;
			if (TinySocket::GetAcceptEx(m_listenSocket, &acceptex))
			{
				PER_IO_CONTEXT* context = new PER_IO_CONTEXT();
				ZeroMemory(context, sizeof(PER_IO_CONTEXT));
				context->OP = OP_ACCEPT;
				context->ListenSocket = m_listenSocket;
				context->AcceptSocket = acceptSocket;
				context->IOCompletion = BindCallback(&TinyAcceptor::OnIOCompletion, this);
				DWORD dwBytes = 0;
				DWORD dwAddressSize = sizeof(SOCKADDR_IN) + 16;
				CHAR data[(sizeof(SOCKADDR_IN) + 16) * 2];
				ZeroMemory(data, (sizeof(SOCKADDR_IN) + 16) * 2);
				LPOVERLAPPED ps = static_cast<LPOVERLAPPED>(context);
				if (!acceptex(m_listenSocket, acceptSocket, data, 0, dwAddressSize, dwAddressSize, &dwBytes, ps) &&
					ERROR_IO_PENDING != WSAGetLastError())
				{
					closesocket(acceptSocket);
					SAFE_DELETE(context);
					return FALSE;
				}
				m_contexts.InsertLast(context);
				return TRUE;
			}
			return FALSE;
		}

		void TinyAcceptor::OnIOCompletion(DWORD dwCode, DWORD dwBytes, PER_IO_CONTEXT*context)
		{
			if (dwCode == 0)
			{
				LPOVERLAPPED ps = reinterpret_cast<LPOVERLAPPED>(context);
				ZeroMemory(ps, sizeof(OVERLAPPED));
				context->Buffer.buf = new CHAR[MAX_BUFFER_SIZE];
				context->Buffer.len = MAX_BUFFER_SIZE;
				DWORD	dwRecvBytes = 0;
				DWORD	dwFlag = 0;
				context->OP = OP_RECV;
				if (WSARecv(context->AcceptSocket, &context->Buffer, 1, &dwRecvBytes, &dwFlag, context, NULL) == SOCKET_ERROR
					&& ERROR_IO_PENDING != WSAGetLastError())
				{
					ITERATOR pos = m_contexts.Lookup(context);
					if (pos)
					{
						m_contexts.RemoveAt(pos);
					}
					SAFE_DELETE(context);
				}
				if (context->OP == OP_ACCEPT)
				{
					this->BeginAccept();
				}
			}
		}
	}
}