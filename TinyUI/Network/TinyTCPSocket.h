#pragma once
#include "TinySocket.h"
#include "TinyIOServer.h"

namespace TinyUI
{
	namespace Network
	{
		class TinyTCPSocket : public TinySocket
		{
			DISALLOW_COPY_AND_ASSIGN(TinyTCPSocket)
		public:
			explicit TinyTCPSocket(TinyIOServer* ioserver);
			virtual ~TinyTCPSocket();
			BOOL Bind(IPAddress& address, DWORD dwPORT);
			BOOL Listen(INT backlog = SOMAXCONN);
			BOOL BeginAccept();
			virtual void OnAccept(DWORD dwCode, DWORD dwNumberOfBytesTransferred, TinySocket* socket);
		private:
			TinyIOServer*	m_ioserver;
		};
	};
}

