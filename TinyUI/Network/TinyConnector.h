#pragma once
#include "TinyNetwork.h"
#include "TinySocket.h"

namespace TinyUI
{
	namespace Network
	{
#define OP_ACCEPT	1
#define OP_RECV		2
#define OP_SEND		3
		class TinyAcceptor;
		/// <summary>
		/// ½ÓÊÕÌ×½Ó×Ö
		/// </summary>
		class TinyConnector : public TinySocket
		{
			friend class TinyAcceptor;
		public:
			TinyConnector(SOCKET socket = INVALID_SOCKET);
			virtual ~TinyConnector();
			BOOL	BeginConnect(SOCKADDR_IN address);
		};
	};
}

