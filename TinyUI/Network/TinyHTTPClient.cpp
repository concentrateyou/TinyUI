#include "../stdafx.h"
#include "TinyHTTPClient.h"
#include "TinyDNS.h"
#include <process.h>

namespace TinyUI
{
	namespace Network
	{
		const CHAR TinyHTTPClient::GET[] = "GET";
		const CHAR TinyHTTPClient::POST[] = "POST";
		const CHAR TinyHTTPClient::HTTP[] = "HTTP";
		const CHAR TinyHTTPClient::HTTP10[] = "HTTP/1.0";
		const CHAR TinyHTTPClient::HTTP11[] = "HTTP/1.1";
		const CHAR TinyHTTPClient::HTTP20[] = "HTTP/2.0";
		const CHAR TinyHTTPClient::HTTPS[] = "HTTPS";
		const CHAR TinyHTTPClient::AcceptCharset[] = "Accept-Charset";
		const CHAR TinyHTTPClient::AcceptEncoding[] = "Accept-Encoding";
		const CHAR TinyHTTPClient::AcceptLanguage[] = "Accept-Language";
		const CHAR TinyHTTPClient::Authorization[] = "Authorization";
		const CHAR TinyHTTPClient::CacheControl[] = "Cache-Control";
		const CHAR TinyHTTPClient::Connection[] = "Connection";
		const CHAR TinyHTTPClient::ContentLength[] = "Content-Length";
		const CHAR TinyHTTPClient::ContentType[] = "Content-Type";
		const CHAR TinyHTTPClient::Cookie[] = "Cookie";
		const CHAR TinyHTTPClient::Host[] = "Host";
		const CHAR TinyHTTPClient::IfModifiedSince[] = "If-Modified-Since";
		const CHAR TinyHTTPClient::IfNoneMatch[] = "If-None-Match";
		const CHAR TinyHTTPClient::IfRange[] = "If-Range";
		const CHAR TinyHTTPClient::Origin[] = "Origin";
		const CHAR TinyHTTPClient::Pragma[] = "Pragma";
		const CHAR TinyHTTPClient::ProxyAuthorization[] = "Proxy-Authorization";
		const CHAR TinyHTTPClient::ProxyConnection[] = "Proxy-Connection";
		const CHAR TinyHTTPClient::Range[] = "Range";
		const CHAR TinyHTTPClient::Referer[] = "Referer";
		const CHAR TinyHTTPClient::TransferEncoding[] = "Transfer-Encoding";
		const CHAR TinyHTTPClient::TokenBinding[] = "Sec-Token-Binding";
		const CHAR TinyHTTPClient::UserAgent[] = "User-Agent";
		const CHAR TinyHTTPClient::CRLF[] = "\r\n";//»Ø³µ»»ÐÐ

		TinyHTTPClient::TinyHTTPClient()
			:m_dwTimeout(3000),
			m_dwOffset(0),
			m_size(0),
			m_errorCode(S_OK),
			m_statusCode(0),
			m_bClose(TRUE)
		{
			m_wait.CreateEvent();
			m_raw.Reset(new CHAR[DEFAULT_HTTP_BUFFER_SIZE]);
		}
		BOOL TinyHTTPClient::Open(const string& szURL, const string& method)
		{
			m_method = std::move(method);
			if (!m_sURL.ParseURL(szURL.c_str(), szURL.size()))
				return FALSE;
			string szHOST = m_sURL.GetComponent(TinyURL::HOST);
			if (szHOST.empty())
				return FALSE;
			Add(Host, szHOST);
			string scheme = m_sURL.GetComponent(TinyURL::SCHEME);
			string port = m_sURL.GetComponent(TinyURL::PORT);
			if (inet_addr(szHOST.c_str()) == INADDR_NONE)
			{
				TinyDNS dns;
				AddressList list;
				if (!dns.Resolver(szHOST, scheme, list))
					return FALSE;
				m_endpoint.FromIPAddress(list[0].address(), port.empty() ? list[0].port() : static_cast<SHORT>(atoi(port.c_str())));
			}
			else
			{
				m_endpoint.FromIPAddress(IPAddress(szHOST), port.empty() ? (strcasecmp(scheme.c_str(), HTTP) == 0 ? 80 : 443) : static_cast<SHORT>(atoi(port.c_str())));
			}
			if (m_socket.Open())
			{
				m_socket.SetDelay(FALSE);
				m_socket.SetTimeout(TRUE, m_dwTimeout);
				m_socket.SetTimeout(FALSE, m_dwTimeout);
				BuildRequest();
				m_socket.BeginConnect(m_endpoint, BindCallback(&TinyHTTPClient::OnHandleConnect, this), this);
				if (m_wait.Lock(INFINITE))
				{
					if (m_errorCode == 0 && m_statusCode == 200)
						return TRUE;
				}
			}
			return FALSE;
		}
		void TinyHTTPClient::SetTimeout(DWORD dwTO)
		{
			m_dwTimeout = dwTO;
		}
		void TinyHTTPClient::Close()
		{
			m_socket.Shutdown();
			m_socket.Close();
		}
		void TinyHTTPClient::SetBody(CHAR* body, INT size)
		{
			m_body.Add(body, size);
		}
		INT TinyHTTPClient::Read(CHAR*& data, INT size)
		{
			m_errorCode = 0;
			m_dwOffset = 0;
			m_size = size;
			if (m_size > DEFAULT_HTTP_BUFFER_SIZE)
			{
				m_raw.Reset(new CHAR[m_size]);
			}
			if (m_raw != NULL)
			{
				if (!m_socket.BeginReceive(m_raw, size, 0, BindCallback(&TinyHTTPClient::OnHandleReceive, this), this))
				{
					OnHandleError(GetLastError());
				}
				if (m_wait.Lock(INFINITE))
				{
					if (m_errorCode == 0)
					{
						data = m_raw;
						return m_size;
					}
				}
			}
			return -1;
		}
		INT	TinyHTTPClient::GetErrorCode() const
		{
			return m_errorCode;
		}
		void TinyHTTPClient::OnHandleConnect(INT errorCode, AsyncResult* result)
		{
			if (errorCode != 0)
			{
				OnHandleError(errorCode);
			}
			else
			{
				m_socket.EndConnect(result);
				if (!m_socket.BeginSend(m_request.GetPointer(), m_request.GetSize(), 0, BindCallback(&TinyHTTPClient::OnHandleRequest, this), this))
				{
					OnHandleError(GetLastError());
				}
			}
		}
		void TinyHTTPClient::OnHandleRequest(INT errorCode, AsyncResult* result)
		{
			if (errorCode != 0)
			{
				OnHandleError(errorCode);
			}
			else
			{
				DWORD dwRes = m_socket.EndSend(result);
				dwRes += m_dwOffset;
				if (dwRes < static_cast<DWORD>(m_request.GetSize()))
				{
					m_dwOffset = dwRes;
					if (!m_socket.BeginSend(m_request.GetPointer() + dwRes, m_request.GetSize() - dwRes, 0, BindCallback(&TinyHTTPClient::OnHandleRequest, this), this))
					{
						OnHandleError(GetLastError());
					}
				}
				else
				{
					if (!m_socket.BeginReceive(m_raw, DEFAULT_HTTP_BUFFER_SIZE, 0, BindCallback(&TinyHTTPClient::OnHandleResponse, this), this))
					{
						OnHandleError(GetLastError());
					}
				}
			}
		}
		void TinyHTTPClient::OnHandleResponse(INT errorCode, AsyncResult* result)
		{
			if (errorCode != 0)
			{
				OnHandleError(errorCode);
			}
			else
			{
				DWORD dwRes = m_socket.EndReceive(result);
				if (dwRes > 0)
				{
					m_response.Add(m_raw, dwRes);
					if (ParseResponse(m_response.GetPointer(), m_response.GetSize()))
					{
						m_wait.SetEvent();
					}
				}
			}
		}
		void TinyHTTPClient::OnHandleReceive(INT errorCode, AsyncResult* result)
		{
			if (errorCode != 0)
			{
				OnHandleError(errorCode);
			}
			else
			{
				DWORD dwRes = m_socket.EndReceive(result);
				if (dwRes > 0)
				{
					m_dwOffset += dwRes;
					if (m_dwOffset >= m_size)
					{
						m_wait.SetEvent();
					}
					else
					{
						if (!m_socket.BeginReceive(m_raw + m_dwOffset, m_size - m_dwOffset, 0, BindCallback(&TinyHTTPClient::OnHandleReceive, this), this))
						{
							OnHandleError(GetLastError());
						}
					}
				}
			}
		}
		BOOL TinyHTTPClient::ParseResponse(CHAR* s, INT size)
		{
			CHAR* line = ReadLine(s);
			if (!ParseStatusLine(s, line - 2))
				return FALSE;
			while (line != NULL)
			{
				CHAR* ps = line;
				line = ReadLine(ps);
				if (*line == '\r' && *(line + 1) == '\n')
					break;
				if (!ParseAttribute(ps, line - 2))
					return FALSE;
			}
			return TRUE;
		}
		BOOL TinyHTTPClient::ParseAttribute(CHAR* ps1, CHAR* ps2)
		{
			if (!ps1 || !ps2)
				return FALSE;
			CHAR* ps3 = ps1;
			while (*ps3 != ':')
				++ps3;
			CHAR* ps4 = ps3;
			while (*ps4 != ' ')
				++ps4;
			ps4++;
			Add(string(ps1, ps3 - ps1), string(ps4, ps2 - ps4));
			return TRUE;
		}

		CHAR* TinyHTTPClient::ReadLine(CHAR* s)
		{
			CHAR* val = s;
			while (*val++ != '\r');
			if (*val++ == '\n')
				return val;
			return NULL;
		}
		BOOL TinyHTTPClient::ParseStatusLine(CHAR* ps1, CHAR* ps2)
		{
			if (!ps1 || !ps2)
				return FALSE;
			if (strncasecmp(ps1, TinyHTTPClient::HTTP10, 8) != 0 &&
				strncasecmp(ps1, TinyHTTPClient::HTTP11, 8) != 0 &&
				strncasecmp(ps1, TinyHTTPClient::HTTP20, 8) != 0)
				return FALSE;
			m_version = string(ps1, 8);
			ps1 += 8;
			while (*ps1 == ' ')
				++ps1;
			CHAR* ps3 = ps1;
			while (IsAsciiDigit(*ps3))
				++ps3;
			string val(ps1, ps3 - ps1);
			m_statusCode = atoi(val.c_str());
			while (*ps3 == ' ')
				++ps3;
			m_desc.append(ps3, ps2);
			return TRUE;
		}
		void TinyHTTPClient::OnHandleError(INT errorCode)
		{
			m_errorCode = errorCode;
			m_wait.SetEvent();
			this->Close();
		}
		void TinyHTTPClient::BuildRequest()
		{
			string szAll = m_sURL.GetComponent(TinyURL::FULLPATH);
			string szRequest = StringPrintf("%s %s %s\r\n", m_method.c_str(), szAll.empty() ? "/" : szAll.c_str(), TinyHTTPClient::HTTP11);
			for (std::vector<KeyValue>::const_iterator s = m_attributes.begin(); s != m_attributes.end(); ++s)
			{
				if (s->key == Connection && s->value == string("close"))
				{
					m_bClose = TRUE;
				}
				if (!s->value.empty())
				{
					szRequest.append(StringPrintf("%s: %s\r\n", s->key.c_str(), s->value.c_str()));
				}
				else
				{
					szRequest.append(StringPrintf("%s:\r\n", s->key.c_str()));
				}
			}
			szRequest.append("\r\n");
			m_request.Clear();
			m_request.Add(&szRequest[0], szRequest.size());
			m_request.Add(m_body.GetPointer(), m_body.GetSize());
		}
	}
}