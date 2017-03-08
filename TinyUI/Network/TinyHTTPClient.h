#pragma once
#include "TinyHTTPRequest.h"
#include "TinyHTTPResponse.h"

namespace TinyUI
{
	namespace Network
	{
#define HTTP_BUFFER_SIZE 8*1024 
		/// <summary>
		/// HTTP¿Í»§¶Ë
		/// https://www.w3.org/Protocols/HTTP/1.0/spec.html
		/// </summary>
		class TinyHTTPClient
		{
			DISALLOW_COPY_AND_ASSIGN(TinyHTTPClient)
		public:
			TinyHTTPClient(TinyIOServer* ioserver = NULL);
			virtual ~TinyHTTPClient();
			BOOL Connect(const IPEndPoint& endpoint);
			BOOL SendRequest(TinyHTTPRequest& request);
		private:
			void OnConnect(DWORD dwError, AsyncResult* result);
			void OnSend(DWORD dwError, AsyncResult* result);
			void OnReceive(DWORD dwError, AsyncResult* result);
			void OnError(DWORD dwError);
		protected:
			CHAR				m_send[HTTP_BUFFER_SIZE];
			CHAR				m_receive[HTTP_BUFFER_SIZE];
			TinySocket			m_socket;
		};
	}
}

