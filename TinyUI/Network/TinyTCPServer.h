#pragma once
#include "TinyNetwork.h"
#include "TinySocket.h"
#include "TinyAcceptor.h"

namespace TinyUI
{
	namespace Network
	{
		class TinyTCPServer
		{
			DISALLOW_COPY_AND_ASSIGN(TinyTCPServer)
		public:
			TinyTCPServer(DWORD dwPORT);
			BOOL Initialize();
			BOOL BeginAccept();
			BOOL Close();
		private:
			TinyScopedPtr<TinyAcceptor> m_acceptor;
			SOCKET			m_listen;
			DWORD			m_dwPORT;
		};
	};
}

