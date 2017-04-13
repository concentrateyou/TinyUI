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
			TinyHTTPMessage();
			virtual ~TinyHTTPMessage();
		public:
			void Add(const string& key, const string& value);
		protected:
			map<string, string> m_map;
		};
	}
}

