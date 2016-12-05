#pragma once
#include "TinySocket.h"

namespace TinyUI
{
	namespace Network
	{
		class TinyHTTPClient
		{
			DISALLOW_COPY_AND_ASSIGN(TinyHTTPClient)
		public:
			TinyHTTPClient(TinyIOServer* ioserver);
			virtual ~TinyHTTPClient();
			BOOL Connect(const IPEndPoint& endpoint);
		private:
			void ConnectCallback(DWORD dwError, AsyncResult* result);
		protected:
			TinySocket			m_socket;
			CompleteCallback	m_connectCB;
		};
	}
}

