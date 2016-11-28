#include "../stdafx.h"
#include "TinyTCPServer.h"
#include <process.h>

namespace TinyUI
{
	namespace Network
	{
		TinyTCPServer::TinyTCPServer(TinyIOServer* pIOServer, DWORD dwPORT)
			:m_pIOServer(pIOServer),
			m_dwPORT(dwPORT),
			m_listen(NULL)
		{

		}
		TinyTCPServer::~TinyTCPServer()
		{

		}
		BOOL TinyTCPServer::Initialize(ULONG_PTR completionKey)
		{
			ASSERT(m_pIOServer);
			m_listen = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			BOOL bRes = m_pIOServer->GetIOCP()->Register(reinterpret_cast<HANDLE>(m_listen), completionKey);
			if (!bRes)
				return FALSE;
			SOCKADDR_IN si;
			ZeroMemory(&si, sizeof(si));
			si.sin_family = AF_INET;
			si.sin_addr.s_addr = htonl(INADDR_ANY);
			si.sin_port = htons(static_cast<USHORT>(m_dwPORT));
			if (bind(m_listen, (sockaddr*)&si, sizeof(si)) == SOCKET_ERROR)
				return FALSE;
			if (listen(m_listen, SOMAXCONN) == SOCKET_ERROR)
				return FALSE;
			m_acceptor.Reset(new TinyAcceptor(m_listen));
			return TRUE;
		}
		BOOL TinyTCPServer::BeginAccept()
		{
			ASSERT(m_acceptor);
			return m_acceptor->BeginAccept();
		}
		BOOL TinyTCPServer::Close()
		{
			return TRUE;
		}
	}
}