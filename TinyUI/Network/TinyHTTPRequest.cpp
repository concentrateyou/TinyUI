#include "../stdafx.h"
#include "TinyHTTPRequest.h"
#include <process.h>

namespace TinyUI
{
	namespace Network
	{
		const char HTTPRequestHeaders::GetMethod[] = "GET";
		const char HTTPRequestHeaders::AcceptCharset[] = "Accept-Charset";
		const char HTTPRequestHeaders::AcceptEncoding[] = "Accept-Encoding";
		const char HTTPRequestHeaders::AcceptLanguage[] = "Accept-Language";
		const char HTTPRequestHeaders::Authorization[] = "Authorization";
		const char HTTPRequestHeaders::CacheControl[] = "Cache-Control";
		const char HTTPRequestHeaders::Connection[] = "Connection";
		const char HTTPRequestHeaders::ContentLength[] = "Content-Length";
		const char HTTPRequestHeaders::ContentType[] = "Content-Type";
		const char HTTPRequestHeaders::Cookie[] = "Cookie";
		const char HTTPRequestHeaders::Host[] = "Host";
		const char HTTPRequestHeaders::IfModifiedSince[] = "If-Modified-Since";
		const char HTTPRequestHeaders::IfNoneMatch[] = "If-None-Match";
		const char HTTPRequestHeaders::IfRange[] = "If-Range";
		const char HTTPRequestHeaders::Origin[] = "Origin";
		const char HTTPRequestHeaders::Pragma[] = "Pragma";
		const char HTTPRequestHeaders::ProxyAuthorization[] = "Proxy-Authorization";
		const char HTTPRequestHeaders::ProxyConnection[] = "Proxy-Connection";
		const char HTTPRequestHeaders::Range[] = "Range";
		const char HTTPRequestHeaders::Referer[] = "Referer";
		const char HTTPRequestHeaders::TransferEncoding[] = "Transfer-Encoding";
		const char HTTPRequestHeaders::TokenBinding[] = "Sec-Token-Binding";
		const char HTTPRequestHeaders::UserAgent[] = "User-Agent";

		HTTPRequestHeaders::HeaderKeyValuePair::HeaderKeyValuePair()
		{
		}

		HTTPRequestHeaders::HeaderKeyValuePair::HeaderKeyValuePair(const string& key, const string& value)
			: key(key.data(), key.size()), value(value.data(), value.size())
		{
		}
	}
}