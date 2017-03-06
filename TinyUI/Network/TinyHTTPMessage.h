#pragma once
#include "TinySocket.h"
#include <map>

namespace TinyUI
{
	namespace Network
	{
		class TinyHTTPMessage
		{
			DISALLOW_COPY_AND_ASSIGN(TinyHTTPMessage)
		public:

		public:
			static const CHAR Method[];
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
			TinyHTTPMessage();
			virtual ~TinyHTTPMessage();
		protected:
			map<string, string> m_map;
		};
	}
}

