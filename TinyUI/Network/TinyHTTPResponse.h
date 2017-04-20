#pragma once
#include "TinyHTTPMessage.h"
#include "TinyURL.h"

namespace TinyUI
{
	namespace Network
	{
		class TinyHTTPResponse : public TinyHTTPMessage
		{
			DISALLOW_COPY_AND_ASSIGN(TinyHTTPResponse)
		public:
			TinyHTTPResponse();
		};
	}
}

