#include "../stdafx.h"
#include "TinyHTTPClient.h"
#include "TinyDNS.h"
#include <process.h>
#include <algorithm>

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
		const CHAR TinyHTTPClient::CRLF[] = "\r\n";
		//////////////////////////////////////////////////////////////////////////
		HTTPRequest::HTTPRequest()
			:m_verbs(TinyHTTPClient::GET)
		{

		}
		HTTPRequest::~HTTPRequest()
		{

		}
		void HTTPRequest::SetVerbs(const string& verbs)
		{
			m_verbs = std::move(verbs);
		}
		string HTTPRequest::GetVerbs() const
		{
			return m_verbs;
		}
		void HTTPRequest::SetBody(const string& body)
		{
			m_body = std::move(body);
		}
		string HTTPRequest::GetBody() const
		{
			return m_body;
		}
		//////////////////////////////////////////////////////////////////////////
		HTTPResponse::HTTPResponse(TinyHTTPClient& client)
			:m_statusCode(0),
			m_client(client)
		{

		}
		HTTPResponse::~HTTPResponse()
		{

		}
		string HTTPResponse::GetVersion() const
		{
			return m_version;
		}
		string HTTPResponse::GetGetStatusMsg() const
		{
			return m_statusMsg;
		}
		INT	 HTTPResponse::GetStatusCode() const
		{
			return m_statusCode;
		}

		BOOL HTTPResponse::ReadAsString(string& val)
		{
			if (!this->Contains(TinyHTTPClient::ContentLength))
			{
				val.resize(m_context.GetSize());
				memcpy(&val[0], m_context.GetPointer(), m_context.GetSize());
				return TRUE;
			}
			else
			{
				INT size = std::stoi(this->GetAttribute(TinyHTTPClient::ContentLength));
				if (size == m_context.GetSize())
				{
					val.resize(size);
					memcpy(&val[0], m_context.GetPointer(), size);
					return TRUE;
				}
				if (size > m_context.GetSize())
				{
					CHAR* ps = NULL;
					INT remainder = size - m_context.GetSize();
					INT iRes = m_client.Read(ps, remainder);
					val.resize(size);
					memcpy(&val[0], m_context.GetPointer(), m_context.GetSize());
					memcpy(&val[0] + m_context.GetSize(), ps, iRes);
					return TRUE;
				}
			}
			return FALSE;
		}

		INT HTTPResponse::ReadAsBinary(CHAR*& ps)
		{
			if (!this->Contains(TinyHTTPClient::ContentLength))
			{
				ps = m_context.GetPointer();
				return m_context.GetSize();
			}
			else
			{
				INT size = std::stoi(this->GetAttribute(TinyHTTPClient::ContentLength));
				if (size == m_context.GetSize())
				{
					ps = m_context.GetPointer();
					return m_context.GetSize();
				}
				if (size > m_context.GetSize())
				{
					CHAR* pValue = NULL;
					INT remainder = size - m_context.GetSize();
					INT iRes = m_client.Read(pValue, remainder);
					m_context.Add(pValue, iRes);
					ps = m_context.GetPointer();
					return size;
				}
			}
			return -1;
		}

		BOOL HTTPResponse::ParseTransferEncoding(CHAR* line)
		{
			while (line != NULL)
			{
				CHAR* ps = line;
				line = ReadLine(ps);
				INT offset = line - ps - 2;
				if (offset > 0)
				{
					INT chunksize = 0;
					sscanf_s(ps, "%x", &chunksize, offset);//16进制转10进制
					if (chunksize == 0)
						break;
					m_context.Add(line, chunksize);//获得Body数据
					line += chunksize;
					continue;
				}
				if (offset == 0)
					break;
				return FALSE;
			}
			return TRUE;
		}

		BOOL HTTPResponse::ParseResponse(CHAR* s, INT size)
		{
			m_context.Clear();
			CHAR* line = ReadLine(s);
			if (!ParseStatusLine(s, line - 2))
				return FALSE;
			while (line != NULL)
			{
				CHAR* ps = line;
				line = ReadLine(ps);
				if (*line == '\r' && *(line + 1) == '\n')//应答头解析完成
				{
					line += 2;
					if (this->Contains(TinyHTTPClient::TransferEncoding))//开始解析TransferEncoding
					{
						if (this->GetAttribute(TinyHTTPClient::TransferEncoding) == "chunked")//目前只处理分块
						{
							if (!ParseTransferEncoding(line))
								return FALSE;
						}
					}
					else
					{
						INT offset = line - s;
						m_context.Add(line, size - offset);//保存剩余数据
					}
					break;
				}
				if (!ParseAttribute(ps, line - 2))
					return FALSE;
			}
			return TRUE;
		}
		BOOL HTTPResponse::ParseStatusLine(CHAR* ps1, CHAR* ps2)
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
			m_statusCode = std::stoi(val);
			while (*ps3 == ' ')
				++ps3;
			m_statusMsg.append(ps3, ps2);
			return TRUE;
		}
		BOOL HTTPResponse::ParseAttribute(CHAR* ps1, CHAR* ps2)
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

		CHAR* HTTPResponse::ReadLine(CHAR* s)
		{
			CHAR* val = s;
			while (*val++ != '\r');
			if (*val++ == '\n')
				return val;
			return NULL;
		}
		//////////////////////////////////////////////////////////////////////////
		TinyHTTPClient::TinyHTTPClient()
			:m_timeout(3000),
			m_offset(0),
			m_size(0),
			m_errorCode(S_OK),
			m_reponse(*this)
		{
			m_wait.CreateEvent();
			m_raw.Reset(new CHAR[DEFAULT_HTTP_BUFFER_SIZE]);
		}
		TinyHTTPClient::~TinyHTTPClient()
		{
			this->Close();
		}
		HTTPRequest& TinyHTTPClient::GetRequest()
		{
			return m_request;
		}
		HTTPResponse& TinyHTTPClient::GetResponse()
		{
			return m_reponse;
		}
		BOOL TinyHTTPClient::Open(const string& szURL)
		{
			if (!m_szURL.ParseURL(szURL.c_str(), szURL.size()))
				return FALSE;
			string szHOST = m_szURL.GetComponent(TinyURL::HOST);
			string szPORT = m_szURL.GetComponent(TinyURL::PORT);
			string sSCHEME = m_szURL.GetComponent(TinyURL::SCHEME);
			if (inet_addr(szHOST.c_str()) == INADDR_NONE)
			{
				TinyDNS dns;
				AddressList list;
				if (!dns.Resolver(szHOST, sSCHEME, list))
					return FALSE;
				m_endpoint.FromIPAddress(list[0].address(), szPORT.empty() ? list[0].port() : static_cast<SHORT>(std::stoi(szPORT)));
			}
			else
			{
				m_endpoint.FromIPAddress(IPAddress(szHOST), szPORT.empty() ? (strcasecmp(sSCHEME.c_str(), HTTP) == 0 ? 80 : 443) : static_cast<SHORT>(std::stoi(szPORT)));
			}
			if (m_socket.Open())
			{
				m_socket.SetDelay(FALSE);
				m_socket.SetTimeout(TRUE, m_timeout);
				m_socket.SetTimeout(FALSE, m_timeout);
				this->BuildRequest();
				m_socket.BeginConnect(m_endpoint, BindCallback(&TinyHTTPClient::OnHandleConnect, this), this);
				if (m_wait.Lock(INFINITE))
				{
					if (m_errorCode == S_OK && m_reponse.GetStatusCode() == 200)
						return TRUE;
				}
			}
			return FALSE;
		}
		void TinyHTTPClient::SetTimeout(DWORD dwTO)
		{
			m_timeout = dwTO;
		}
		void TinyHTTPClient::Close()
		{
			m_socket.Shutdown();
			m_socket.Close();
		}
		INT TinyHTTPClient::Read(CHAR*& data, INT size)
		{
			m_errorCode = S_OK;
			m_offset = 0;
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
				if (m_wait.Lock(INFINITE) && m_errorCode == S_OK)
				{
					data = m_raw;
					return m_size;
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
				if (!m_socket.BeginSend(m_requests.GetPointer(), m_requests.GetSize(), 0, BindCallback(&TinyHTTPClient::OnHandleRequest, this), this))
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
				dwRes += static_cast<DWORD>(m_offset);
				if (dwRes < static_cast<DWORD>(m_requests.GetSize()))
				{
					m_offset = static_cast<INT>(dwRes);
					if (!m_socket.BeginSend(m_requests.GetPointer() + dwRes, m_requests.GetSize() - dwRes, 0, BindCallback(&TinyHTTPClient::OnHandleRequest, this), this))
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
					m_reponse.ParseResponse(m_raw, dwRes);
				}
				m_wait.SetEvent();
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
					m_offset += dwRes;
					if (m_offset >= m_size)
					{
						m_wait.SetEvent();
					}
					else
					{
						if (!m_socket.BeginReceive(m_raw + m_offset, m_size - m_offset, 0, BindCallback(&TinyHTTPClient::OnHandleReceive, this), this))
						{
							OnHandleError(GetLastError());
						}
					}
				}
			}
		}
		void TinyHTTPClient::OnHandleError(INT errorCode)
		{
			m_errorCode = errorCode;
			m_wait.SetEvent();
			this->Close();
		}
		void TinyHTTPClient::BuildRequest()
		{
			string szHOST = m_szURL.GetComponent(TinyURL::HOST);
			string szPORT = m_szURL.GetComponent(TinyURL::PORT);
			m_request.Add(TinyHTTPClient::Host, szPORT.empty() ? szHOST : StringPrintf("%s:%s", szHOST.c_str(), szPORT.c_str()));
			string body = m_request.GetBody();
			if (!body.empty())
			{
				m_request.Add(TinyHTTPClient::ContentLength, std::to_string(body.size()));
			}
			string szFullPath = m_szURL.GetComponent(TinyURL::FULLPATH);
			string szRequest = StringPrintf("%s %s %s\r\n", m_request.GetVerbs().c_str(), szFullPath.empty() ? "/" : szFullPath.c_str(), TinyHTTPClient::HTTP11);//请求行
			szRequest += m_request.ToString();
			szRequest += m_request.GetBody();
			m_requests.Clear();
			m_requests.Add(&szRequest[0], szRequest.size());
		}
	}
}