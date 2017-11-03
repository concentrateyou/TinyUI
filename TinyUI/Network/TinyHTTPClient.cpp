#include "../stdafx.h"
#include "TinyHTTPClient.h"
#include "../Common/TinyLogging.h"
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
				if (this->GetAttribute(TinyHTTPClient::TransferEncoding) == "chunked")//开始解析TransferEncoding
				{
					if (m_context.GetSize() == 0)
					{
						CHAR* ps = NULL;
						INT size = m_client.ReadSome(ps);
						if (!ParseTransferEncoding(ps))
							return FALSE;
					}
				}
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
				if (this->GetAttribute(TinyHTTPClient::TransferEncoding) == "chunked")//开始解析TransferEncoding
				{
					if (m_context.GetSize() == 0)
					{
						CHAR* ps = NULL;
						INT size = m_client.ReadSome(ps);
						if (!ParseTransferEncoding(ps))
							return FALSE;
					}
				}
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
					sscanf(ps, "%x", &chunksize);//16进制转10进制
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
					if (this->GetAttribute(TinyHTTPClient::TransferEncoding) == "chunked")//开始解析TransferEncoding
					{
						INT offset = line - s;
						if (offset < size)
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
			:m_timeout(20000),
			m_reponse(*this),
			m_socket(INVALID_SOCKET),
			m_size(DEFAULT_HTTP_BUFFER_SIZE)
		{
			m_raw.Reset(new CHAR[m_size]);
			ZeroMemory(m_raw, m_size);
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
		BOOL TinyHTTPClient::IsEmpty() const
		{
			return (m_socket == NULL) || (m_socket == INVALID_SOCKET);
		}
		BOOL TinyHTTPClient::Open(const string& szURL)
		{
			if (!m_szURL.ParseURL(szURL.c_str(), szURL.size()))
				return FALSE;
			size_t size = 0;
			SOCKADDR s = { 0 };
			ULONG iMode = 1;
			timeval val;
			val.tv_sec = 5;//5秒超时
			val.tv_usec = 0;
			fd_set set;
			FD_ZERO(&set);
			string szHOST = m_szURL.GetComponent(TinyURL::HOST);
			string szPORT = m_szURL.GetComponent(TinyURL::PORT);
			string sSCHEME = m_szURL.GetComponent(TinyURL::SCHEME);
			if (inet_addr(szHOST.c_str()) == INADDR_NONE)
			{
				TinyDNS dns;
				AddressList list;
				if (!dns.Resolver(szHOST, sSCHEME, list))
					return FALSE;
				if (list.size() > 1)
				{
					INT index = 0;
					for (size_t i = 0; i < list.size();i++)
					{
						if (list[i].address().IsIPv4())
						{
							index = i;
							break;
						}
					}
					m_endpoint.FromIPAddress(list[index].address(), szPORT.empty() ? list[index].port() : static_cast<SHORT>(std::stoi(szPORT)));
				}
				else
				{
					m_endpoint.FromIPAddress(list[0].address(), szPORT.empty() ? list[0].port() : static_cast<SHORT>(std::stoi(szPORT)));
				}
			}
			else
			{
				m_endpoint.FromIPAddress(IPAddress(szHOST), szPORT.empty() ? (strcasecmp(sSCHEME.c_str(), HTTP) == 0 ? 80 : 443) : static_cast<SHORT>(std::stoi(szPORT)));
			}
			this->BuildRequest();
			m_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (m_socket == INVALID_SOCKET)
			{
				LOG(ERROR) << "socket == INVALID_SOCKET";
				goto _ERROR;
			}



			BOOL bAllow = TRUE;
			INT iRes = setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, (const CHAR*)&bAllow, sizeof(bAllow));
			if (iRes != 0)
			{
				LOG(ERROR) << "setsockopt TCP_NODELAY:" << WSAGetLastError();
				goto _ERROR;
			}
			INT buffer = 512 * 1024;
			iRes = ::setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (const CHAR*)&buffer, sizeof(buffer));
			if (iRes != 0)
			{
				LOG(ERROR) << "setsockopt SO_RCVBUF:" << WSAGetLastError();
				goto _ERROR;
			}
			iRes = ::setsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, (const CHAR*)&buffer, sizeof(buffer));
			if (iRes != 0)
			{
				LOG(ERROR) << "setsockopt SO_SNDBUF:" << WSAGetLastError();
				goto _ERROR;
			}
			iRes = ::setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (CHAR *)&m_timeout, sizeof(m_timeout));
			if (iRes != 0)
			{
				LOG(ERROR) << "setsockopt SO_RCVTIMEO:" << WSAGetLastError();
				goto _ERROR;
			}
			iRes = ::setsockopt(m_socket, SOL_SOCKET, SO_SNDTIMEO, (CHAR *)&m_timeout, sizeof(m_timeout));
			if (iRes != 0)
			{
				LOG(ERROR) << "setsockopt SO_SNDTIMEO:" << WSAGetLastError();
				goto _ERROR;
			}
			if (!m_endpoint.ToSOCKADDR(&s, &size))
			{
				LOG(ERROR) << "ToSOCKADDR FAIL";
				goto _ERROR;
			}
			iRes = ::ioctlsocket(m_socket, FIONBIO, &iMode);//设置异步模式
			if (iRes != 0)
			{
				LOG(ERROR) << "ioctlsocket FIONBIO Mode = 1:" << WSAGetLastError();
				goto _ERROR;
			}
			if (::connect(m_socket, (LPSOCKADDR)&s, sizeof(s)) == 0)
			{
				LOG(ERROR) << "socket connect:" << WSAGetLastError();
				goto _ERROR;
			}
			iMode = 0;
			iRes = ioctlsocket(m_socket, FIONBIO, &iMode); //设置阻塞模式
			if (iRes != 0)
			{
				LOG(ERROR) << "ioctlsocket FIONBIO Mode = 0:" << WSAGetLastError();
				goto _ERROR;
			}
			FD_SET(m_socket, &set);
			select(0, NULL, &set, NULL, &val);
			if (FD_ISSET(m_socket, &set))
			{
				iRes = this->Write(m_requests.GetPointer(), m_requests.GetSize());
				if (iRes == SOCKET_ERROR || iRes != m_requests.GetSize())
				{
					LOG(ERROR) << "socket send:" << GetLastError();
					goto _ERROR;
				}
				ZeroMemory(m_raw, DEFAULT_HTTP_BUFFER_SIZE);
				CHAR* bits = NULL;
				iRes = this->ReadSome(bits);
				if (iRes == SOCKET_ERROR)
				{
					LOG(ERROR) << "socket recv:" << GetLastError();
					goto _ERROR;
				}
				if (m_reponse.ParseResponse(bits, iRes) && m_reponse.GetStatusCode() == 200)
				{
					return TRUE;
				}
			}
		_ERROR:
			Close();
			return FALSE;
		}
		INT	TinyHTTPClient::Write(CHAR* bits, INT size)
		{
			INT value = 0;
			for (;;)
			{
				if (value >= size)
					break;
				INT iRes = ::send(m_socket, bits + value, size - value, 0);
				if (iRes == SOCKET_ERROR)
				{
					LOG(ERROR) << "[TinyHTTPClient] Write FAIL:" << WSAGetLastError();
					return iRes;
				}
				value += iRes;
			}
			return value;
		}
		INT	TinyHTTPClient::Read(CHAR*& bits, INT size)
		{
			if (size > m_size)
			{
				m_size = size;
				m_raw.Reset(new CHAR[m_size]);
				ZeroMemory(m_raw, m_size);
			}
			INT value = 0;
			for (;;)
			{
				if (value >= size)
					break;
				INT iRes = ::recv(m_socket, m_raw + value, size - value, 0);
				if (iRes == SOCKET_ERROR)
				{
					LOG(ERROR) << "[TinyHTTPClient] Read FAIL:" << WSAGetLastError();
					bits = NULL;
					return iRes;
				}
				value += iRes;
			}
			bits = m_raw;
			return value;
		}
		INT TinyHTTPClient::ReadSome(CHAR*& bits)
		{
			ZeroMemory(m_raw, m_size);
			INT iRes = ::recv(m_socket, m_raw, m_size, 0);
			if (iRes == SOCKET_ERROR)
			{
				LOG(ERROR) << "[TinyHTTPClient] ReadSome FAIL:" << WSAGetLastError();
				bits = NULL;
				return iRes;
			}
			bits = m_raw;
			return iRes;
		}
		void TinyHTTPClient::Close()
		{
			if (m_socket != NULL && m_socket != INVALID_SOCKET)
			{
				::shutdown(m_socket, SD_BOTH);
				::closesocket(m_socket);
			}
			m_socket = INVALID_SOCKET;
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