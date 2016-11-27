#pragma once
#include "TinyNetwork.h"
#include "TinySocket.h"
#include "TinyIOServer.h"

namespace TinyUI
{
	namespace Network
	{
		class NO_VTABLE ACCEPT_IO_CONTEXT : public IO_CONTEXT
		{
		public:
			SOCKET	socket;
		};

		class TinyAcceptor : public TinyReference<TinyAcceptor>, public TinyIOTask
		{
		public:
			TinyAcceptor(SOCKET listen);
		public:
			BOOL	BeginAccept(SOCKET socket);
			ACCEPT_IO_CONTEXT*	EndAccept();
			void OnCompletionStatus(IO_CONTEXT* pIO, DWORD dwError) OVERRIDE;
		private:
			SOCKET m_listen;
		};
	}
}

