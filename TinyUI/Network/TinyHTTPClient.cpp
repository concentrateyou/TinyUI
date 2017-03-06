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
			return m_socket.BeginConnect(endpoint, BindCallback(&TinyHTTPClient::OnConnect, this), this);
		}
		BOOL TinyHTTPClient::SendRequest(TinyHTTPRequest& request)
		{
			if (m_socket.IsConnect())
			{
				m_socket.BeginSend();
			}
		}
		void TinyHTTPClient::OnConnect(DWORD dwError, AsyncResult* result)
		{
			if (dwError != 0)
			{
				OnError(dwError);
			}
			else
			{
				m_socket.EndConnect(result);
			}
		}
		void TinyHTTPClient::OnSend(DWORD dwError, AsyncResult* result)
		{
			if (dwError != 0)
			{
				OnError(dwError);
			}
		}
		void TinyHTTPClient::OnReceive(DWORD dwError, AsyncResult* result)
		{
			if (dwError != 0)
			{
				OnError(dwError);
			}
		}
		void TinyHTTPClient::OnError(DWORD dwError)
		{

		}
	}
}