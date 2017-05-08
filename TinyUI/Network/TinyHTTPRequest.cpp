#include "../stdafx.h"
#include "TinyHTTPRequest.h"
#include "TinyHTTPResponse.h"
#include "TinyDNS.h"
#include <process.h>

namespace TinyUI
{
	namespace Network
	{
		const CHAR TinyHTTPRequest::GET[] = "GET";
		const CHAR TinyHTTPRequest::POST[] = "POST";
		const CHAR TinyHTTPRequest::HTTP[] = "HTTP";
		const CHAR TinyHTTPRequest::HTTP10[] = "HTTP/1.0";
		const CHAR TinyHTTPRequest::HTTP11[] = "HTTP/1.1";
		const CHAR TinyHTTPRequest::HTTP20[] = "HTTP/2.0";
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
			m_dwOffset(0),
			m_dwError(S_OK),
			m_bClose(TRUE)
		{
			m_raw.Reset(new CHAR[8192]);
			m_event.CreateEvent();
		}
		BOOL TinyHTTPRequest::Open(const string& szURL, const string& ms)
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
			m_line = StringPrintf("%s %s %s\r\n", m_ms.c_str(), all.empty() ? "/" : all.c_str(), TinyHTTPRequest::HTTP11);
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
		void TinyHTTPRequest::Close()
		{
			m_socket.Shutdown();
			m_socket.Close();
		}
		TinyHTTPResponse* TinyHTTPRequest::GetResponse()
		{
			if (m_socket.IsValid())
			{
				BuildRequest();
				m_socket.BeginConnect(m_endpoint, BindCallback(&TinyHTTPRequest::OnHandleConnect, this), this);
				if (m_event.Lock(INFINITE) && m_dwError == S_OK)
				{
					TinyHTTPResponse* response = new TinyHTTPResponse();
					if (response->ParseResponse(m_response.GetPointer(), m_response.GetSize()))
					{
						return response;
					}
					response->Close();
				}
			}
			return NULL;
		}
		void TinyHTTPRequest::SetBody(CHAR* ps, INT size)
		{
			m_body.Clear();
			Add(ContentLength, StringPrintf("%d", size));
			m_body.Add(ps, size);
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
				dwRes += m_dwOffset;
				if (dwRes < static_cast<DWORD>(m_request.GetSize()))
				{
					m_dwOffset = dwRes;
					if (!m_socket.BeginSend(m_request.GetPointer() + dwRes, m_request.GetSize() - dwRes, 0, BindCallback(&TinyHTTPRequest::OnHandleSend, this), this))
					{
						OnHandleError(GetLastError());
					}
				}
				else
				{
					if (!m_socket.BeginReceive(m_raw, 8192, 0, BindCallback(&TinyHTTPRequest::OnHandleReceive, this), this))
					{
						OnHandleError(GetLastError());
					}
				}
			}
		}
		void TinyHTTPRequest::OnHandleReceive(DWORD dwError, AsyncResult* result)
		{
			if (dwError != 0)
			{
				OnHandleError(dwError);
			}
			else
			{
				DWORD dwRes = m_socket.EndReceive(result);
				if (dwRes > 0)
				{
					m_response.Add(m_raw, dwRes);
					if (!m_socket.BeginReceive(m_raw, 8192, 0, BindCallback(&TinyHTTPRequest::OnHandleReceive, this), this))
					{
						OnHandleError(GetLastError());
					}
				}
				else
				{
					if (m_bClose)
					{
						this->Close();
					}
					m_event.SetEvent();
				}
			}
		}
		void TinyHTTPRequest::OnHandleError(DWORD dwError)
		{
			m_dwError = dwError;
			this->Close();
			m_event.SetEvent();
		}
		void TinyHTTPRequest::BuildRequest()
		{
			string output = std::move(m_line);
			for (std::vector<KeyValue>::const_iterator s = m_attributes.begin(); s != m_attributes.end(); ++s)
			{
				if (s->key == Connection && s->value == string("close"))
				{
					m_bClose = TRUE;
				}
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
			m_request.Clear();
			m_request.Add(&output[0], output.size());
			m_request.Add(m_body.GetPointer(), m_body.GetSize());
			m_body.Clear();
		}
	}
}