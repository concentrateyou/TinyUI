#include "../stdafx.h"
#include "TinyHTTPClient.h"
#include "TinyDNS.h"
#include "TinyURL.h"

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

		BOOL TinyHTTPClient::GetURL(const string& val)
		{
			TinyURL url;
			if (!url.ParseURL(val.c_str(), val.size()))
				return FALSE;
			string host = url.GetComponent(TinyURL::HOST);
			if (inet_addr(host.c_str()) == INADDR_NONE)
			{
				string scheme = url.GetComponent(TinyURL::SCHEME);
				TinyDNS dns;
				AddressList list;
				if (!dns.Resolver(host, scheme, list))
					return FALSE;
				IPEndPoint endpoint = list[0];
				//string val = endpoint.ToString();
				//INT a = 0;
			}
			return TRUE;
		}

		BOOL TinyHTTPClient::PostURL(const string& val)
		{
			return TRUE;
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