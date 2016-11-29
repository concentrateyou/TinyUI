#pragma once
#include "TinyNetwork.h"
#include "TinySocket.h"
#include "TinyIOServer.h"

namespace TinyUI
{
	namespace Network
	{
		class TinyAcceptor
		{
		public:
			explicit TinyAcceptor(SOCKET listen);
			virtual ~TinyAcceptor();
			void	Close();
		public:
			BOOL	BeginAccept();
			virtual void OnIOCompletion(DWORD dwCode, DWORD dwBytes, PER_IO_CONTEXT*ps);
		private:
			SOCKET	m_listenSocket;
			TinyLinkList<PER_IO_CONTEXT*>	m_contexts;
		};
	}
}

