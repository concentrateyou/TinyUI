#pragma once
#include "TinySocket.h"

namespace TinyUI
{
	namespace Network
	{
#define HTTP_BUFFER_SIZE 8192

		class TinyHTTPClient
		{
			DISALLOW_COPY_AND_ASSIGN(TinyHTTPClient)
		public:
			TinyHTTPClient(TinyIOServer* ioserver = NULL);
			virtual ~TinyHTTPClient();
			BOOL Connect(const IPEndPoint& endpoint);
		private:
			void OnConnect(DWORD dwError, AsyncResult* result);
			void OnSend(DWORD dwError, AsyncResult* result);
			void OnReceive(DWORD dwError, AsyncResult* result);
			void OnError(DWORD dwError);
		protected:
			CHAR				m_sendDATA[HTTP_BUFFER_SIZE];
			CHAR				m_receiveDATA[HTTP_BUFFER_SIZE];
			TinySocket			m_socket;
			CompleteCallback	m_connectCB;
		};
	}
}

