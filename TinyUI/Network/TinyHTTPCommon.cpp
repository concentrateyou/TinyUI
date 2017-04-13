#include "../stdafx.h"
#include "TinyHTTPCommon.h"

namespace TinyUI
{
	namespace Network
	{
		static const char* HttpVersions[HTTP_VERSION_LAST + 1] =
		{
			"1.0", "1.1", "Unknown"
		};

		static const char* HttpVerbs[HTTP_VERB_LAST + 1] =
		{
			"GET", "POST", "PUT", "DELETE", "CONNECT", "HEAD"
		};

		static const char* HttpHeaders[HTTP_HEADER_LAST + 1] = 
		{
			"Age",
			"Cache-Control",
			"Connection",
			"Content-Disposition",
			"Content-Length",
			"Content-Range",
			"Content-Type",
			"Cookie",
			"Date",
			"ETag",
			"Expires",
			"Host",
			"If-Modified-Since",
			"If-None-Match",
			"Keep-Alive",
			"Last-Modified",
			"Location",
			"Proxy-Authenticate",
			"Proxy-Authorization",
			"Proxy-Connection",
			"Range",
			"Set-Cookie",
			"TE",
			"Trailers",
			"Transfer-Encoding",
			"Upgrade",
			"User-Agent",
			"WWW-Authenticate",
		};
	}
}