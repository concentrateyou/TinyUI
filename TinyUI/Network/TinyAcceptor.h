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
		public:
			BOOL	BeginAccept();
			virtual void IOCompletion(DWORD dwCode, DWORD dwBytes, IO_CONTEXT*ps);
		private:
			SOCKET	m_listen;
			CHAR	m_data[1024];
		};
	}
}

