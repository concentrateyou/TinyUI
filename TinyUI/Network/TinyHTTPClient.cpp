#include "../stdafx.h"
#include "TinyHTTPClient.h"
#include "../Common/TinyLogging.h"


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
		IOBuffer::IOBuffer()
			:m_data(NULL)
		{

		}
		IOBuffer::IOBuffer(INT size)
			: m_data(NULL)
		{
			m_data = new CHAR[size];
		}
		IOBuffer::IOBuffer(CHAR* data)
			: m_data(data)
		{

		}
		IOBuffer::~IOBuffer()
		{
			SAFE_DELETE_ARRAY(m_data);
		}
		CHAR* IOBuffer::data() const
		{
			return m_data;
		}
		//////////////////////////////////////////////////////////////////////////
		DrainableIOBuffer::DrainableIOBuffer(IOBuffer* base, INT size)
			:IOBuffer(base->data()),
			m_base(base),
			m_size(size),
			m_offset(0)
		{

		}
		void DrainableIOBuffer::SetOffset(INT offset)
		{
			m_offset = offset;
			m_data = m_base->data() + offset;
		}
		INT  DrainableIOBuffer::GetOffset() const
		{
			return m_offset;
		}
		INT  DrainableIOBuffer::GetRemaining() const
		{
			return m_size - m_offset;
		}
		INT DrainableIOBuffer::size() const
		{
			return m_size;
		}
		DrainableIOBuffer::~DrainableIOBuffer()
		{
			m_data = NULL;
		}
		//////////////////////////////////////////////////////////////////////////
		GrowableIOBuffer::GrowableIOBuffer()
			:m_capacity(0),
			m_offset(0)
		{

		}
		void GrowableIOBuffer::SetCapacity(INT capacity)
		{
			m_readIO.reset(static_cast<CHAR*>(realloc(m_readIO.release(), capacity)));
			m_capacity = capacity;
			if (m_offset > capacity)
			{
				SetOffset(capacity);
			}
			else
			{
				SetOffset(m_offset);
				ZeroMemory(m_data, m_capacity - m_offset);
			}
		}
		INT GrowableIOBuffer::capacity() const
		{
			return m_capacity;
		}
		void GrowableIOBuffer::SetOffset(INT offset)
		{
			m_offset = offset;
			m_data = m_readIO.get() + offset;
		}
		INT GrowableIOBuffer::offset() const
		{
			return m_offset;
		}
		INT GrowableIOBuffer::RemainingCapacity()
		{
			return m_capacity - m_offset;
		}
		CHAR* GrowableIOBuffer::StartOfBuffer()
		{
			return m_readIO.get();
		}
		GrowableIOBuffer::~GrowableIOBuffer()
		{
			m_data = NULL;
		}
		//////////////////////////////////////////////////////////////////////////
		HTTPRequest::HTTPRequest()
			:m_verbs(TinyHTTPClient::GET),
			m_bodySize(0)
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
			m_body.Reset(new CHAR[body.size()]);
			m_bodySize = body.size();
			memcpy(m_body, &body[0], body.size());
		}
		void HTTPRequest::SetBody(const CHAR* bits, LONG size)
		{
			m_body.Reset(new CHAR[size]);
			m_bodySize = size;
			memcpy(m_body, bits, size);
		}
		CHAR* HTTPRequest::GetBody() const
		{
			return m_body;
		}
		LONG HTTPRequest::GetBodySize() const
		{
			return m_bodySize;
		}
		//////////////////////////////////////////////////////////////////////////
		HTTPResponse::HTTPResponse(TinyHTTPClient& client)
			:m_statusCode(0),
			m_client(client)
		{
			m_growIO = new GrowableIOBuffer();
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
					if (!ParseTransferEncoding())
						return FALSE;
					val.resize(m_context.GetSize());
					memcpy_s(&val[0], m_context.GetSize(), m_context.GetPointer(), m_context.GetSize());
				}
			}
			else
			{
				INT length = std::stoi(this->GetAttribute(TinyHTTPClient::ContentLength));
				if (length > 0)
				{
					INT remaining = m_growIO->RemainingCapacity();
					if (remaining > 0)
					{
						m_context.Add(m_growIO->data(), remaining);
						m_growIO->SetOffset(m_growIO->offset() + remaining);
					}
					INT size = length - remaining;
					TinyScopedPtr<CHAR> buffer(new CHAR[size]);
					ASSERT(buffer);
					INT iRes = m_client.Read(buffer, size);
					if (iRes == SOCKET_ERROR || iRes != size)
						return FALSE;
					m_context.Add(buffer, iRes);
					val.resize(m_context.GetSize());
					memcpy_s(&val[0], m_context.GetSize(), m_context.GetPointer(), m_context.GetSize());
				}
			}
			return TRUE;
		}

		INT HTTPResponse::ReadAsBinary(CHAR*& bits)
		{
			m_context.Clear();
			if (!this->Contains(TinyHTTPClient::ContentLength))
			{
				if (this->GetAttribute(TinyHTTPClient::TransferEncoding) == "chunked")//开始解析TransferEncoding
				{
					if (!ParseTransferEncoding())
						return FALSE;
				}
				bits = m_context.GetPointer();
				return m_context.GetSize();
			}
			else
			{
				INT length = std::stoi(this->GetAttribute(TinyHTTPClient::ContentLength));
				if (length > 0)
				{
					INT remaining = m_growIO->RemainingCapacity();
					if (remaining > 0)
					{
						m_context.Add(m_growIO->data(), remaining);
						m_growIO->SetOffset(m_growIO->offset() + remaining);
					}
					INT size = length - remaining;
					TinyScopedPtr<CHAR> buffer(new CHAR[size]);
					ASSERT(buffer);
					INT iRes = m_client.Read(buffer, size);
					if (iRes == SOCKET_ERROR || iRes != size)
						return -1;
					m_context.Add(buffer, iRes);
					bits = m_context.GetPointer();
					return length;
				}
			}
			return -1;
		}

		BOOL HTTPResponse::ParseTransferEncoding()
		{
			for (;;)
			{
				CHAR* line1 = NULL;
				CHAR* line2 = NULL;
				if (!ReadLine(line1, line2))
					return FALSE;
				if (*line1 == '\r' && *(line1 + 1) == '\n')
					continue;
				INT chunksize = 0;
				sscanf(line1, "%x", &chunksize);//16进制转10进制
				if (chunksize == 0)
					break;
				INT remaining = m_growIO->RemainingCapacity();
				if (remaining >= chunksize)
				{
					m_context.Add(line2, chunksize);//获得Body数据
					m_growIO->SetOffset(m_growIO->offset() + chunksize);
				}
				else
				{
					m_growIO->SetCapacity(m_growIO->capacity() + chunksize - remaining);
					INT iRes = m_client.Read(m_growIO->data(), m_growIO->RemainingCapacity());
					if (iRes == SOCKET_ERROR && iRes != m_growIO->RemainingCapacity())
						return FALSE;
					m_context.Add(m_growIO->data(), chunksize - remaining);
					m_growIO->SetOffset(m_growIO->offset() + (chunksize - remaining));
				}
			}
			return TRUE;
		}

		BOOL HTTPResponse::ParseResponse()
		{
			CHAR* line1 = NULL;
			CHAR* line2 = NULL;
			if (!ReadLine(line1, line2))
				return FALSE;
			if (!ParseStatusLine(line1, line2 - 2))
				return FALSE;
			for (;;)
			{
				if (!ReadLine(line1, line2))
					return FALSE;
				if (*line1 == '\r' && *(line1 + 1) == '\n')
				{
					m_growIO->SetOffset(m_growIO->offset() - (line2 - line1) + 2);
					break;
				}
				if (!ParseAttribute(line1, line2 - 2))
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
		BOOL HTTPResponse::FindLine(INT& size)
		{
			CHAR* bits = m_growIO->data();
			if (*bits++ == '\n')
			{
				size = bits - m_growIO->data();
				return TRUE;
			}
			if (size == (bits - m_growIO->data()))
			{
				return FALSE;
			}
			bits = m_growIO->data();
			while (*bits++ != '\r')
			{
				if (size == (bits - m_growIO->data()))
				{
					return FALSE;
				}
			}
			if (*bits++ == '\n')
			{
				size = bits - m_growIO->data();
				return TRUE;
			}
			return FALSE;
		}
		BOOL HTTPResponse::ReadLine(CHAR*& line1, CHAR*& line2)
		{
			INT iRes = 0;
			INT offset = m_growIO->offset();
			for (;;)
			{
				iRes = m_growIO->RemainingCapacity();
				if (iRes == 0)
				{
					m_growIO->SetCapacity(m_growIO->capacity() + DEFALUT_HTTP_HEADER_INITIAL_SIZE);
					iRes = m_client.ReadSome(m_growIO->data(), m_growIO->RemainingCapacity());
					if (iRes == SOCKET_ERROR)
						return FALSE;
				}
				if (!FindLine(iRes))
				{
					m_growIO->SetOffset(m_growIO->offset() + iRes);
					continue;
				}
				m_growIO->SetOffset(m_growIO->offset() + iRes);
				line1 = m_growIO->StartOfBuffer() + offset;
				line2 = m_growIO->StartOfBuffer() + m_growIO->offset();
				return TRUE;
			}
			return FALSE;
		}
		//////////////////////////////////////////////////////////////////////////
		TinyHTTPClient::TinyHTTPClient()
			:m_timeout(20000),
			m_reponse(*this)
		{
		}
		TinyHTTPClient::~TinyHTTPClient()
		{
			this->Close();
		}
		void TinyHTTPClient::SetTimeout(INT timeout)
		{
			m_timeout = timeout;
		}
		void TinyHTTPClient::SetErrorCallback(ErrorCallback&& callback)
		{
			m_socket.SetErrorCallback(std::move(callback));
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
			return m_socket.IsEmpty();
		}
		BOOL TinyHTTPClient::Open(const string& szURL)
		{
			if (!m_szURL.ParseURL(szURL.c_str(), szURL.size()))
				return FALSE;
			INT size = 0;
			SOCKADDR s = { 0 };
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
			if (!m_socket.Open(AF_INET, SOCK_STREAM, IPPROTO_TCP))
			{
				LOG(ERROR) << "socket Open:" << WSAGetLastError();
				goto _ERROR;
			}
			if (!m_socket.SetDelay(FALSE))
			{
				LOG(ERROR) << "socket SetNoDelay:" << WSAGetLastError();
				goto _ERROR;
			}
			size = 512 * 1024;
			if (!m_socket.SetOption(SOL_SOCKET, SO_RCVBUF, (const CHAR*)&size, sizeof(size)))
			{
				LOG(ERROR) << "setsockopt SO_RCVBUF:" << WSAGetLastError();
				goto _ERROR;
			}
			if (!m_socket.SetOption(SOL_SOCKET, SO_SNDBUF, (const CHAR*)&size, sizeof(size)))
			{
				LOG(ERROR) << "socket SO_SNDBUF:" << WSAGetLastError();
				goto _ERROR;
			}
			if (!m_socket.SetTimeout(TRUE, m_timeout))
			{
				LOG(ERROR) << "socket SetTimeout:" << WSAGetLastError();
				goto _ERROR;
			}
			if (!m_socket.SetTimeout(FALSE, m_timeout))
			{
				LOG(ERROR) << "socket SetTimeout:" << WSAGetLastError();
				goto _ERROR;
			}
			if (!m_endpoint.ToSOCKADDR(&s, reinterpret_cast<size_t*>(&size)))
			{
				LOG(ERROR) << "ToSOCKADDR FAIL";
				goto _ERROR;
			}
			if (!m_socket.Connect(m_endpoint, 3))//连接3秒超时
			{
				LOG(ERROR) << "socket connect:" << WSAGetLastError();
				goto _ERROR;
			}
			INT iRes = this->Write(m_requestIO.GetPointer(), m_requestIO.GetSize());
			if (iRes == SOCKET_ERROR || iRes != m_requestIO.GetSize())
			{
				LOG(ERROR) << "socket send:" << GetLastError();
				goto _ERROR;
			}
			if (m_reponse.ParseResponse() && m_reponse.GetStatusCode() == 200)
			{
				return TRUE;
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
				INT iRes = m_socket.Send(bits + value, size - value, 0);
				if (iRes == SOCKET_ERROR)
				{
					LOG(ERROR) << "[TinyHTTPClient] Write FAIL:" << WSAGetLastError();
					return iRes;
				}
				value += iRes;
			}
			return value;
		}
		INT TinyHTTPClient::ReadSome(CHAR* bits, INT size)
		{
			INT iRes = m_socket.Receive(bits, size, 0);
			if (iRes == 0)//对方关闭Socket连接
			{
				return SOCKET_ERROR;
			}
			return iRes;
		}
		INT	TinyHTTPClient::Read(CHAR* bits, INT size)
		{
			CHAR* ps = bits;
			INT value = 0;
			for (;;)
			{
				if (value >= size)
					break;
				INT iRes = m_socket.Receive(ps + value, size - value, 0);
				if (iRes == SOCKET_ERROR)
				{
					return iRes;
				}
				if (iRes == 0)//对方关闭Socket连接
				{
					TRACE("DATA:%d\n", value);
					return SOCKET_ERROR;
				}
				value += iRes;
			}
			return value;
		}
		void TinyHTTPClient::Close()
		{
			m_socket.Close();
		}
		void TinyHTTPClient::BuildRequest()
		{
			string szHOST = m_szURL.GetComponent(TinyURL::HOST);
			string szPORT = m_szURL.GetComponent(TinyURL::PORT);
			m_request.Add(TinyHTTPClient::Host, szPORT.empty() ? szHOST : StringPrintf("%s:%s", szHOST.c_str(), szPORT.c_str()));
			if (!m_request.GetBodySize() != 0)
			{
				m_request.Add(TinyHTTPClient::ContentLength, std::to_string(m_request.GetBodySize()));
			}
			string szFullPath = m_szURL.GetComponent(TinyURL::FULLPATH);
			string szRequest = StringPrintf("%s %s %s\r\n", m_request.GetVerbs().c_str(), szFullPath.empty() ? "/" : szFullPath.c_str(), TinyHTTPClient::HTTP11);//请求行
			szRequest += m_request.ToString();
			m_requestIO.Clear();
			m_requestIO.Add(&szRequest[0], szRequest.size());
			m_requestIO.Add(m_request.GetBody(), m_request.GetBodySize());
		}
	}
}