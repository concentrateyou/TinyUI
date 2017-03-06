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
		void TinyHTTPClient::OnConnect(DWORD dwError, AsyncResult* result)
		{
			if (dwError != 0)
			{
				OnError(dwError);
			}
			else
			{
				m_socket.BeginReceive(m_receiveDATA, HTTP_BUFFER_SIZE, 0, BindCallback(&TinyHTTPClient::OnReceive, this), this);
			}
		}
		void TinyHTTPClient::OnSend(DWORD dwError, AsyncResult* result)
		{
			TinySocket::StreamAsyncResult* async = static_cast<TinySocket::StreamAsyncResult*>(result);
			if (dwError != 0)
			{
				OnError(dwError);
			}
			else
			{

			}
		}
		void TinyHTTPClient::OnReceive(DWORD dwError, AsyncResult* result)
		{
			TinySocket::StreamAsyncResult* async = static_cast<TinySocket::StreamAsyncResult*>(result);
			if (dwError != 0)
			{
				OnError(dwError);
			}
			else
			{
				DWORD dwBytesTransferred = async->BytesTransferred;
				m_socket.BeginReceive(m_receiveDATA, HTTP_BUFFER_SIZE, 0, BindCallback(&TinyHTTPClient::OnReceive, this), this);
			}
		}
		void TinyHTTPClient::OnError(DWORD dwError)
		{

		}
	}
}