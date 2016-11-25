#pragma once
#include "TinyNetwork.h"
#include "TinySocket.h"

namespace TinyUI
{
	namespace Network
	{
#define OP_ACCEPT	0x01
#define OP_RECV		0x02
#define OP_SEND		0x03
		/// <summary>
		/// ½ÓÊÕÌ×½Ó×Ö
		/// </summary>
		class TinyAcceptor : public TinySocket
		{
		public:
			TinyAcceptor(IO::TinyIOCP& iocp, SOCKET socket);
			BOOL	Open(SOCKADDR_IN address);
			BOOL	BeginAccept();
		protected:
			SOCKET			m_socket;
			IO::TinyIOCP&	m_iocp;
		};
	}
}

