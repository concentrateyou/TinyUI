#include "../stdafx.h"
#include "TinyTCPServer.h"
#include <process.h>

namespace TinyUI
{
	namespace Network
	{
		TinyTCPServer::TinyTCPServer(TinyIOServer* pIOServer, DWORD dwPORT)
			:m_pIOServer(pIOServer),
			m_dwPORT(dwPORT)
		{

		}
		TinyTCPServer::~TinyTCPServer()
		{

		}
		BOOL TinyTCPServer::Initialize(ULONG_PTR completionKey)
		{
			ASSERT(m_pIOServer);
			m_listen = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			BOOL bRes = m_pIOServer->GetIOCP()->Register((HANDLE)m_listen, completionKey);
			if (!bRes)
				return FALSE;
			SOCKADDR_IN si;
			ZeroMemory(&si, sizeof(si));
			si.sin_family = AF_INET;
			si.sin_port = ntohs(m_dwPORT);
			si.sin_addr.S_un.S_addr = INADDR_ANY;
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