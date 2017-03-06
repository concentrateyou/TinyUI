#pragma once
#include "TinyHTTPMessage.h"

namespace TinyUI
{
	namespace Network
	{
		class TinyHTTPRequest : public TinyHTTPMessage
		{
		public:
			TinyHTTPRequest();
		public:
			void SetVersion(const string& version);
		};
	}
}

