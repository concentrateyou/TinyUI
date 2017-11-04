#pragma once
#include "TinyNetwork.h"
#include "TinyHTTPCommon.h"
#include "TinyURL.h"
#include <string>
#include <map>

namespace TinyUI
{
	namespace Network
	{
#define DEFAULT_HTTP_BUFFER_SIZE (1024*1024)
		class TinyHTTPClient;
		/// <summary>
		/// 请求
		/// </summary>
		class HTTPRequest : public TinyHTTPAttribute
		{
			friend class TinyHTTPClient;
		protected:
			HTTPRequest();
			~HTTPRequest();
		public:
			void SetBody(const string& body);
			string GetBody() const;
			void SetVerbs(const string& verb);
			string GetVerbs() const;
		private:
			string	m_verbs;
			string	m_body;
		};
		/// <summary>
		/// 应答
		/// </summary>
		class HTTPResponse : public TinyHTTPAttribute
		{
			friend class TinyHTTPClient;
		protected:
			HTTPResponse(TinyHTTPClient& client);
			~HTTPResponse();
		public:
			INT		GetStatusCode() const;
			string	GetVersion() const;
			string	GetGetStatusMsg() const;
			BOOL	ReadAsString(string& val);
			INT		ReadAsBinary(CHAR*& ps);
		private:
			CHAR* ReadLine(CHAR* s);
			BOOL ParseTransferEncoding(CHAR* line);
			BOOL ParseAttribute(CHAR* ps1, CHAR* ps2);
			BOOL ParseResponse(CHAR* s, INT size);
			BOOL ParseStatusLine(CHAR* ps1, CHAR* ps2);
		private:
			INT						m_statusCode;
			string					m_version;//版本
			string					m_statusMsg;//状态信息
			TinyBufferArray<CHAR>	m_context;
			TinyHTTPClient&			m_client;
		};
		/// <summary>
		/// HTTP客户端
		/// </summary>
		class TinyHTTPClient
		{
		public:
			static const CHAR GET[];
			static const CHAR POST[];
			static const CHAR HTTP[];
			static const CHAR HTTP10[];
			static const CHAR HTTP11[];
			static const CHAR HTTP20[];
			static const CHAR HTTPS[];
			static const CHAR AcceptCharset[];
			static const CHAR AcceptEncoding[];
			static const CHAR AcceptLanguage[];
			static const CHAR Authorization[];
			static const CHAR CacheControl[];
			static const CHAR Connection[];
			static const CHAR ContentType[];
			static const CHAR Cookie[];
			static const CHAR ContentLength[];
			static const CHAR Host[];
			static const CHAR IfModifiedSince[];
			static const CHAR IfNoneMatch[];
			static const CHAR IfRange[];
			static const CHAR Origin[];
			static const CHAR Pragma[];
			static const CHAR ProxyAuthorization[];
			static const CHAR ProxyConnection[];
			static const CHAR Range[];
			static const CHAR Referer[];
			static const CHAR TransferEncoding[];
			static const CHAR TokenBinding[];
			static const CHAR UserAgent[];
			static const CHAR CRLF[];
			enum State
			{
				STATE_NONE,
				STATE_SEND_HEADERS,
				STATE_SEND_HEADERS_COMPLETE,
				STATE_SEND_BODY,
				STATE_SEND_BODY_COMPLETE,
				STATE_SEND_REQUEST_READ_BODY_COMPLETE,
				STATE_READ_HEADERS,
				STATE_READ_HEADERS_COMPLETE,
				STATE_READ_BODY,
				STATE_READ_BODY_COMPLETE,
				STATE_DONE
			};
			DISALLOW_COPY_AND_ASSIGN(TinyHTTPClient)
		public:
			TinyHTTPClient();
			~TinyHTTPClient();
			BOOL			IsEmpty() const;
			BOOL			Open(const string& szURL);
			void			Close();
			INT				Write(CHAR* bits, INT size);
			INT				Read(CHAR*& bits, INT size);
			INT				ReadSome(CHAR*& bits);
			HTTPRequest&	GetRequest();
			HTTPResponse&	GetResponse();
		private:
			void BuildRequest();
		private:
			INT						m_timeout;
			TinyURL					m_szURL;
			TinySocket				m_socket;
			IPEndPoint				m_endpoint;
			HTTPRequest				m_request;
			HTTPResponse			m_reponse;
			LONG					m_size;
			TinyScopedArray<CHAR>	m_raw;
			TinyBufferArray<CHAR>	m_requests;
		};
	}
}

