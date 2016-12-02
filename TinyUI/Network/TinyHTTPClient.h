#pragma once
#include "TinyNetwork.h"
#include <winhttp.h>
#pragma comment(lib,"Winhttp.lib")

namespace TinyUI
{
	namespace Network
	{
		class TinyHTTPClient
		{
			DISALLOW_COPY_AND_ASSIGN(TinyHTTPClient)
		public:
			TinyHTTPClient();
			virtual ~TinyHTTPClient();
		};
	}
}

