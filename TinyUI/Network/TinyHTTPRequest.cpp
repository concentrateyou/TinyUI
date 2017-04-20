#include "../stdafx.h"
#include "TinyHTTPRequest.h"
#include "TinyHTTPResponse.h"
#include "TinyDNS.h"
#include <process.h>

namespace TinyUI
{
	namespace Network
	{
		static const string HTTP = "http";
		static const string HTTPS = "https";

		TinyHTTPRequest::TinyHTTPRequest()
			:m_ms("GET"),
			m_version("1.1")
		{

		}

		BOOL TinyHTTPRequest::Create(const string& szURL)
		{
			if (!m_sURL.ParseURL(szURL.c_str(),szURL.size()))
				return FALSE;
			string scheme = m_sURL.GetComponent(TinyURL::SCHEME);
			string port = m_sURL.GetComponent(TinyURL::PORT);
			string host = m_sURL.GetComponent(TinyURL::HOST);
			if (inet_addr(host.c_str()) == INADDR_NONE)
			{
				TinyDNS dns;
				AddressList list;
				if (!dns.Resolver(host, scheme, list))
					return FALSE;
				m_endpoint.FromIPAddress(list[0].address(), port.empty() ? list[0].port() : static_cast<SHORT>(atoi(port.c_str())));
			}
			else
			{
				m_endpoint.FromIPAddress(IPAddress(host), port.empty() ? (scheme == HTTP ? 80 : 443) : static_cast<SHORT>(atoi(port.c_str())));
			}
			return TRUE;
		}

		void TinyHTTPRequest::SetGeneral(const string& ms, const string& version)
		{
			m_ms = std::move(ms);
			m_version = std::move(version);
		}

		TinyHTTPResponse* TinyHTTPRequest::GetResponse()
		{
			m_socket.Close();
			if (m_socket.Open())
			{
				m_socket.Connect(m_endpoint);
				TinyHTTPResponse* response = new TinyHTTPResponse();
				return response;
			}
			return NULL;
		}

		string TinyHTTPRequest::Build()
		{
			string scheme = m_sURL.GetComponent(TinyURL::SCHEME);
			string host = m_sURL.GetComponent(TinyURL::HOST);
			string path = m_sURL.GetComponent(TinyURL::PATH);
			string val;
			if (path.empty())
			{
				val = StringPrintf("%s %s/%s\r\n", m_ms.c_str(), scheme.c_str(), m_version.c_str());
			}
			else
			{
				val = StringPrintf("%s %s %s/%s\r\n", m_ms.c_str(), path.c_str(), scheme.c_str(), m_version.c_str());
			}
			val += StringPrintf("Host:%s\r\n", host.c_str());
			for (auto v : m_map)
			{
				val += StringPrintf("%s:%s\r\n", v.first.c_str(), v.second.c_str());
			}
			val += "\r\n";

			return val;
		}
	}
}