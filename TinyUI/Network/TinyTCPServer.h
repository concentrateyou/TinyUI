#pragma once
#include "TinySocket.h"
#include "TinyIOServer.h"

namespace TinyUI
{
	namespace Network
	{
		/// <summary>
		/// TCP·þÎñÆ÷
		/// </summary>
		class TinyTCPServer : public TinySocket
		{
			DISALLOW_COPY_AND_ASSIGN(TinyTCPServer)
		public:
			explicit TinyTCPServer(TinyIOServer* ioserver);
			virtual ~TinyTCPServer();
			BOOL Initialize(DWORD dwPORT);
			BOOL BeginAccept(CompleteCallback& callback);
		private:
			TinyIOServer*	m_ioserver;
		};
	};
}

