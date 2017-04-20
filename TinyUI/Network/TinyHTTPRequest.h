#pragma once
#include "TinyNetwork.h"
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
			DISALLOW_COPY_AND_ASSIGN(TinyHTTPRequest);
			struct KeyValue
			{
				KeyValue();
				KeyValue(const std::string& k, const std::string& v);
				KeyValue(KeyValue&& other);
				KeyValue& operator = (KeyValue&& other);
				std::string key;
				std::string value;
			};
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
			BOOL Create(const string& szURL, const string& requestMS);
			void Add(const string& key, const string& val);
			void Remove(const string& key);
			void SetContext(const string& value);
			void GetResponse();
		private:
			std::vector<TinyHTTPRequest::KeyValue>::const_iterator Lookup(const string& key) const;
			std::vector<TinyHTTPRequest::KeyValue>::iterator Lookup(const string& key);
		private:
			string					m_requestMS;
			string					m_context;
			TinyURL					m_sURL;
			TinySocket				m_socket;
			IPEndPoint				m_endpoint;
			std::vector<KeyValue>	m_attributes;
		};
	}
}

