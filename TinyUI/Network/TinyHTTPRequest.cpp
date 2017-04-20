#include "../stdafx.h"
#include "TinyHTTPRequest.h"
#include "TinyHTTPResponse.h"
#include "TinyDNS.h"
#include <process.h>

namespace TinyUI
{
	namespace Network
	{
		TinyHTTPRequest::KeyValue::KeyValue()
		{

		}
		TinyHTTPRequest::KeyValue::KeyValue(const std::string& k, const std::string& v)
			: key(std::move(k)),
			value(std::move(v))
		{

		}
		TinyHTTPRequest::KeyValue::KeyValue(TinyHTTPRequest::KeyValue&& other)
			: key(std::move(other.key)),
			value(std::move(other.value))
		{

		}
		TinyHTTPRequest::KeyValue& TinyHTTPRequest::KeyValue::operator = (TinyHTTPRequest::KeyValue&& other)
		{
			if (this != &other)
			{
				key = std::move(other.key);
				value = std::move(other.value);
				other.key.clear();
				other.value.clear();
			}
			return *this;
		}
		//////////////////////////////////////////////////////////////////////////
		const CHAR TinyHTTPRequest::GET[] = "GET";
		const CHAR TinyHTTPRequest::POST[] = "POST";
		const CHAR TinyHTTPRequest::HTTP[] = "HTTP";
		const CHAR TinyHTTPRequest::HTTP10[] = "HTTP/1.0";
		const CHAR TinyHTTPRequest::HTTP11[] = "HTTP/1.1";
		const CHAR TinyHTTPRequest::HTTPS[] = "HTTPS";
		const CHAR TinyHTTPRequest::AcceptCharset[] = "Accept-Charset";
		const CHAR TinyHTTPRequest::AcceptEncoding[] = "Accept-Encoding";
		const CHAR TinyHTTPRequest::AcceptLanguage[] = "Accept-Language";
		const CHAR TinyHTTPRequest::Authorization[] = "Authorization";
		const CHAR TinyHTTPRequest::CacheControl[] = "Cache-Control";
		const CHAR TinyHTTPRequest::Connection[] = "Connection";
		const CHAR TinyHTTPRequest::ContentLength[] = "Content-Length";
		const CHAR TinyHTTPRequest::ContentType[] = "Content-Type";
		const CHAR TinyHTTPRequest::Cookie[] = "Cookie";
		const CHAR TinyHTTPRequest::Host[] = "Host";
		const CHAR TinyHTTPRequest::IfModifiedSince[] = "If-Modified-Since";
		const CHAR TinyHTTPRequest::IfNoneMatch[] = "If-None-Match";
		const CHAR TinyHTTPRequest::IfRange[] = "If-Range";
		const CHAR TinyHTTPRequest::Origin[] = "Origin";
		const CHAR TinyHTTPRequest::Pragma[] = "Pragma";
		const CHAR TinyHTTPRequest::ProxyAuthorization[] = "Proxy-Authorization";
		const CHAR TinyHTTPRequest::ProxyConnection[] = "Proxy-Connection";
		const CHAR TinyHTTPRequest::Range[] = "Range";
		const CHAR TinyHTTPRequest::Referer[] = "Referer";
		const CHAR TinyHTTPRequest::TransferEncoding[] = "Transfer-Encoding";
		const CHAR TinyHTTPRequest::TokenBinding[] = "Sec-Token-Binding";
		const CHAR TinyHTTPRequest::UserAgent[] = "User-Agent";
		TinyHTTPRequest::TinyHTTPRequest()
		{

		}

		BOOL TinyHTTPRequest::Create(const string& szURL, const string& requestMS)
		{
			m_requestMS = std::move(requestMS);
			if (!m_sURL.ParseURL(szURL.c_str(), szURL.size()))
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
				m_endpoint.FromIPAddress(IPAddress(host), port.empty() ? (strcasecmp(scheme.c_str(), HTTP) == 0 ? 80 : 443) : static_cast<SHORT>(atoi(port.c_str())));
			}
			return TRUE;
		}

		void TinyHTTPRequest::Remove(const string& key)
		{
			std::vector<KeyValue>::const_iterator s = Lookup(key);
			if (s != m_attributes.end())
			{
				m_attributes.erase(s);
			}
		}

		void TinyHTTPRequest::GetResponse()
		{
			m_socket.Close();
			if (m_socket.Open())
			{
				m_socket.Connect(m_endpoint);
				string scheme = m_sURL.GetComponent(TinyURL::SCHEME);
				string host = m_sURL.GetComponent(TinyURL::HOST);
				if (!host.empty())
				{
					Add(Host, host);
				}
				string path = m_sURL.GetComponent(TinyURL::PATH);
				string query = m_sURL.GetComponent(TinyURL::QUERY);
				string ref = m_sURL.GetComponent(TinyURL::REF);
				string all = path + query + ref;
				string request = StringPrintf("%s %s HTTP/1.1\r\n", m_requestMS.c_str(), all.empty() ? "/" : all.c_str());
				for (std::vector<KeyValue>::const_iterator s = m_attributes.begin(); s != m_attributes.end(); ++s)
				{
					request += StringPrintf("%s:%s\r\n", s->key.c_str(), s->value.c_str());
				}
				request += "\r\n";
				request += m_context;
				INT s = m_socket.Send(&request[0], request.size(), 0);
			}
		}
		void TinyHTTPRequest::Add(const string& key, const string& value)
		{
			std::vector<TinyHTTPRequest::KeyValue>::iterator s = Lookup(key);
			if (s != m_attributes.end())
			{
				s->value = std::move(value);
			}
			else
			{
				m_attributes.push_back(KeyValue(key, value));
			}
		}

		void TinyHTTPRequest::SetContext(const string& value)
		{
			Add(ContentLength, StringPrintf("%d", value.size()));
			m_context = std::move(value);
		}

		std::vector<TinyHTTPRequest::KeyValue>::const_iterator TinyHTTPRequest::Lookup(const string& key) const
		{
			for (std::vector<TinyHTTPRequest::KeyValue>::const_iterator s = m_attributes.begin(); s != m_attributes.end(); ++s)
			{
				if (strncasecmp(key.c_str(), s->key.c_str(), key.size()) == 0)
				{
					return s;
				}
			}
			return m_attributes.end();
		}

		std::vector<TinyHTTPRequest::KeyValue>::iterator TinyHTTPRequest::Lookup(const string& key)
		{
			for (std::vector<TinyHTTPRequest::KeyValue>::iterator s = m_attributes.begin(); s != m_attributes.end(); ++s)
			{
				if (strncasecmp(key.c_str(), s->key.c_str(), key.size()) == 0)
				{
					return s;
				}
			}
			return m_attributes.end();
		}
	}
}