#include "../stdafx.h"
#include "TinyHTTPMessage.h"

namespace TinyUI
{
	namespace Network
	{
		const char TinyHTTPMessage::Method[] = "GET";
		const char TinyHTTPMessage::AcceptCharset[] = "Accept-Charset";
		const char TinyHTTPMessage::AcceptEncoding[] = "Accept-Encoding";
		const char TinyHTTPMessage::AcceptLanguage[] = "Accept-Language";
		const char TinyHTTPMessage::Authorization[] = "Authorization";
		const char TinyHTTPMessage::CacheControl[] = "Cache-Control";
		const char TinyHTTPMessage::Connection[] = "Connection";
		const char TinyHTTPMessage::ContentLength[] = "Content-Length";
		const char TinyHTTPMessage::ContentType[] = "Content-Type";
		const char TinyHTTPMessage::Cookie[] = "Cookie";
		const char TinyHTTPMessage::Host[] = "Host";
		const char TinyHTTPMessage::IfModifiedSince[] = "If-Modified-Since";
		const char TinyHTTPMessage::IfNoneMatch[] = "If-None-Match";
		const char TinyHTTPMessage::IfRange[] = "If-Range";
		const char TinyHTTPMessage::Origin[] = "Origin";
		const char TinyHTTPMessage::Pragma[] = "Pragma";
		const char TinyHTTPMessage::ProxyAuthorization[] = "Proxy-Authorization";
		const char TinyHTTPMessage::ProxyConnection[] = "Proxy-Connection";
		const char TinyHTTPMessage::Range[] = "Range";
		const char TinyHTTPMessage::Referer[] = "Referer";
		const char TinyHTTPMessage::TransferEncoding[] = "Transfer-Encoding";
		const char TinyHTTPMessage::TokenBinding[] = "Sec-Token-Binding";
		const char TinyHTTPMessage::UserAgent[] = "User-Agent";

		TinyHTTPMessage::TinyHTTPMessage()
		{

		}
		TinyHTTPMessage::~TinyHTTPMessage()
		{

		}
		void TinyHTTPMessage::SetVersion(const string& version)
		{
		
		}
		void TinyHTTPMessage::SetContent(const std::string& type, LONG size)
		{
			
		}
		void TinyHTTPMessage::SetKeepAlive(BOOL keepAlive)
		{
			
		}
		void TinyHTTPMessage::SetEncoding(const std::string& encoding)
		{

		}
	}
}