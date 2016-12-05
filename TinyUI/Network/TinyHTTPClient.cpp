#include "../stdafx.h"
#include "TinyHTTPClient.h"

namespace TinyUI
{
	namespace Network
	{
		TinyHTTPClient::TinyHTTPClient(TinyIOServer* ioserver)
			:m_socket(ioserver)
		{

		}
		TinyHTTPClient::~TinyHTTPClient()
		{

		}
		BOOL TinyHTTPClient::Connect(const IPEndPoint& endpoint)
		{
			if (!m_socket.Open())
				return FALSE;
			return TRUE;
		}
		void TinyHTTPClient::ConnectCallback(DWORD dwError, AsyncResult* result)
		{

		}
	}
}