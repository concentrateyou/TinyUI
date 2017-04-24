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
		class TinyHTTPResponse;

		class TinyHTTPRequest : public TinyHTTPAttribute
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
			DISALLOW_COPY_AND_ASSIGN(TinyHTTPRequest)
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
		public:
			TinyHTTPRequest();
			BOOL Create(const string& szURL, const string& ms);
			void Close();
			void SetTimeout(DWORD dwTimeout);	
			void SetBody(CHAR* ps, INT size);
			TinyHTTPResponse* GetResponse();
		private:
			void BuildRequest();
			void OnHandleConnect(DWORD, AsyncResult*);
			void OnHandleSend(DWORD, AsyncResult*);
			void OnHandleReceive(DWORD, AsyncResult*);
			void OnHandleError(DWORD);
		private:
			BOOL					m_bClose;
			DWORD					m_dwError;
			DWORD					m_dwOffset;
			DWORD					m_dwTO;
			string					m_ms;
			string					m_line;
			TinyEvent				m_event;
			TinyURL					m_sURL;
			TinySocket				m_socket;
			IPEndPoint				m_endpoint;
			TinyBufferArray<CHAR>	m_request;
			TinyBufferArray<CHAR>	m_body;
			TinyScopedArray<CHAR>	m_buffer;
			TinyBufferArray<CHAR>	m_response;
		};
	}
}

