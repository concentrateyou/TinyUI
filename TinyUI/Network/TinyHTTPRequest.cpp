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
			:m_dwTO(3000),
			m_dwOffset(0)
		{

		}

		BOOL TinyHTTPRequest::Create(const string& szURL, const string& ms)
		{
			m_ms = std::move(ms);
			if (!m_sURL.ParseURL(szURL.c_str(), szURL.size()))
				return FALSE;
			string host = m_sURL.GetComponent(TinyURL::HOST);
			if (host.empty())
				return FALSE;
			Add(Host, host);
			string scheme = m_sURL.GetComponent(TinyURL::SCHEME);
			string port = m_sURL.GetComponent(TinyURL::PORT);
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
			string all = m_sURL.GetComponent(TinyURL::FULLPATH);
			m_line = StringPrintf("%s %s HTTP/1.1\r\n", m_ms.c_str(), all.empty() ? "/" : all.c_str());
			if (m_socket.Open())
			{
				m_socket.SetDelay(FALSE);
				m_socket.SetTimeout(TRUE, m_dwTO);
				m_socket.SetTimeout(FALSE, m_dwTO);
				return TRUE;
			}
			return FALSE;
		}
		void TinyHTTPRequest::SetTimeout(DWORD dwTO)
		{
			m_dwTO = dwTO;
		}
		void TinyHTTPRequest::Remove(const string& key)
		{
			std::vector<KeyValue>::const_iterator s = Lookup(key);
			if (s != m_attributes.end())
			{
				m_attributes.erase(s);
			}
		}
		string TinyHTTPRequest::operator[](const string& key)
		{
			std::vector<KeyValue>::const_iterator s = Lookup(key);
			if (s != m_attributes.end())
			{
				return s->value;
			}
			return string();
		}
		void TinyHTTPRequest::GetResponse()
		{
			if (m_socket.IsValid())
			{
				string output = std::move(m_line);
				for (std::vector<KeyValue>::const_iterator s = m_attributes.begin(); s != m_attributes.end(); ++s)
				{
					if (!s->value.empty())
					{
						output.append(StringPrintf("%s: %s\r\n", s->key.c_str(), s->value.c_str()));
					}
					else
					{
						output.append(StringPrintf("%s:\r\n", s->key.c_str()));
					}
				}
				output.append("\r\n");
				m_request.Add(&output[0], output.size());
				m_request.Add(m_body.GetPointer(), m_body.GetSize());
				m_socket.BeginConnect(m_endpoint, BindCallback(&TinyHTTPRequest::OnHandleConnect, this), this);
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
		void TinyHTTPRequest::SetContext(CHAR* ps, INT size)
		{
			Add(ContentLength, StringPrintf("%d", size));
			m_body.Add(ps, size);
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

		void TinyHTTPRequest::OnHandleConnect(DWORD dwError, AsyncResult* result)
		{
			if (dwError != 0)
			{
				OnHandleError(dwError);
			}
			else
			{
				m_socket.EndConnect(result);
				if (!m_socket.BeginSend(m_request.GetPointer(), m_request.GetSize(), 0, BindCallback(&TinyHTTPRequest::OnHandleSend, this), this))
				{
					OnHandleError(GetLastError());
				}
			}
		}

		void TinyHTTPRequest::OnHandleSend(DWORD dwError, AsyncResult* result)
		{
			if (dwError != 0)
			{
				OnHandleError(dwError);
			}
			else
			{
				DWORD dwRes = m_socket.EndSend(result);
				if (dwRes < static_cast<DWORD>(m_request.GetSize()))
				{
					m_dwOffset = dwRes;
					if (!m_socket.BeginSend(m_request.GetPointer() + dwRes, m_request.GetSize() - dwRes, 0, BindCallback(&TinyHTTPRequest::OnHandleSend, this), this))
					{
						OnHandleError(GetLastError());
					}
				}
			}
		}
		void TinyHTTPRequest::OnHandleError(DWORD dwError)
		{

		}
	}
}