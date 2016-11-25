#pragma once
#include "../Common//TinyCollection.h"
#include "../IO/TinyIO.h"
#include "../Common/TinyCallback.h"
#include "../IO/TinyIOCP.h"
#include "../IO/TinyTaskBase.h"
#include <WinSock2.h>
#include <mswsock.h>
#include <string>
#include <vector>
using namespace std;
#pragma comment(lib,"Mswsock.lib")
#pragma comment(lib,"Ws2_32.lib")

namespace TinyUI
{
	namespace Network
	{
		class HTTPRequestHeaders
		{
		public:
			struct HeaderKeyValuePair
			{
				HeaderKeyValuePair();
				HeaderKeyValuePair(const string& key, const string& value);
				std::string key;
				std::string value;
			};
			typedef std::vector<HeaderKeyValuePair> HeaderVector;
			static const char GetMethod[];
			static const char PostMethod[];
			static const char AcceptCharset[];
			static const char AcceptEncoding[];
			static const char AcceptLanguage[];
			static const char Authorization[];
			static const char CacheControl[];
			static const char Connection[];
			static const char ContentType[];
			static const char Cookie[];
			static const char ContentLength[];
			static const char Host[];
			static const char IfModifiedSince[];
			static const char IfNoneMatch[];
			static const char IfRange[];
			static const char Origin[];
			static const char Pragma[];
			static const char ProxyAuthorization[];
			static const char ProxyConnection[];
			static const char Range[];
			static const char Referer[];
			static const char TransferEncoding[];
			static const char TokenBinding[];
			static const char UserAgent[];
		//public:
		//	HTTPRequestHeaders();
		//	HTTPRequestHeaders(const HTTPRequestHeaders& other);
		//	~HTTPRequestHeaders();
		//	BOOL IsEmpty() const;
		//	BOOL Contain(const string& key) const;
		//	BOOL Find(const string& key, std::string* out) const;
		//	void Clear();
		//	void Add(const string& key, const string& value);
		//	void Remove(const string& key);
		};
	}
}

