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

#define DEFAULT_HTTP_BUFFER_SIZE 16384

		class TinyHTTPClient : public TinyHTTPAttribute
		{
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
		public:
			TinyHTTPClient();
			BOOL Open(const string& szURL, const string& method = GET);
			void Close();
			void SetTimeout(DWORD dwTO);
			void SetBody(CHAR* body, INT size);
			INT Read(CHAR*& data, INT size);
		private:
			void BuildRequest();
			void OnHandleConnect(INT, AsyncResult*);
			void OnHandleRequest(INT, AsyncResult*);
			void OnHandleResponse(INT, AsyncResult*);
			void OnHandleReceive(INT, AsyncResult*);
			void OnHandleError(INT);
		private:
			CHAR* ReadLine(CHAR* s);
			BOOL ParseAttribute(CHAR* ps1, CHAR* ps2);
			BOOL ParseResponse(CHAR* s, INT size);
			BOOL ParseStatusLine(CHAR* ps1, CHAR* ps2);
		private:
			BOOL					m_bClose;
			DWORD					m_dwOffset;
			DWORD					m_dwTimeout;
			INT						m_errorCode;
			INT						m_statusCode;
			INT						m_size;
			string					m_version;
			string					m_method;
			string					m_desc;
			TinyEvent				m_wait;
			TinyURL					m_sURL;
			TinySocket				m_socket;
			IPEndPoint				m_endpoint;
			TinyScopedArray<CHAR>	m_raw;
			TinyBufferArray<CHAR>   m_body;
			TinyBufferArray<CHAR>   m_response;
			TinyBufferArray<CHAR>	m_request;
		};
	}
}

