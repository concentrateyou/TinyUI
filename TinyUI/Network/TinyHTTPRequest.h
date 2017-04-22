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

		class TinyHTTPRequest
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
			struct KeyValue
			{
				KeyValue();
				KeyValue(const std::string& k, const std::string& v);
				KeyValue(KeyValue&& other);
				KeyValue& operator= (KeyValue&& other);
				std::string key;
				std::string value;
			};
			DISALLOW_COPY_AND_ASSIGN(TinyHTTPRequest)
		public:
			static const CHAR GET[];
			static const CHAR POST[];
			static const CHAR HTTP[];
			static const CHAR HTTP10[];
			static const CHAR HTTP11[];
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
			void SetTimeout(DWORD dwTimeout);
			void Add(const string& key, const string& val);
			void Remove(const string& key);
			void SetContext(CHAR* ps, INT size);
			void GetResponse();
		public:
			string	operator[](const string& key);
		private:
			std::vector<TinyHTTPRequest::KeyValue>::const_iterator Lookup(const string& key) const;
			std::vector<TinyHTTPRequest::KeyValue>::iterator Lookup(const string& key);
		private:
			void OnHandleConnect(DWORD, AsyncResult*);
			void OnHandleSend(DWORD, AsyncResult*);
			void OnHandleReceive(DWORD, AsyncResult*);
			void OnHandleError(DWORD);
		private:
			DWORD					m_dwOffset;
			DWORD					m_dwTO;
			string					m_ms;
			string					m_line;
			TinyURL					m_sURL;
			TinySocket				m_socket;
			IPEndPoint				m_endpoint;
			std::vector<KeyValue>	m_attributes;
			TinyBufferArray<CHAR>	m_request;
			TinyBufferArray<CHAR>	m_body;
			TinyScopedArray<CHAR>	m_buffer;
			TinyBufferArray<CHAR>	m_response;
		};
	}
}

