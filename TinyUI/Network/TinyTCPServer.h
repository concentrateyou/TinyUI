#pragma once
#include "TinyNetwork.h"
#include "TinyIOServer.h"
#include "TinyAcceptor.h"

namespace TinyUI
{
	namespace Network
	{
		class TinyTCPServer
		{
			DISALLOW_COPY_AND_ASSIGN(TinyTCPServer)
		public:
			TinyTCPServer(TinyIOServer* pIOServer, DWORD dwPORT);
			virtual ~TinyTCPServer();
			BOOL Initialize(ULONG_PTR completionKey = 0);
			BOOL BeginAccept();
			BOOL Close();
		private:
			TinyIOServer*				m_pIOServer;
			TinyScopedPtr<TinyAcceptor> m_acceptor;
			SOCKET			m_listen;
			DWORD			m_dwPORT;
		};
	};
}

