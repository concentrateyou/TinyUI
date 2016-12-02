#pragma once
#include "TinySocket.h"

namespace TinyUI
{
	namespace Network
	{
		class TinyHTTPClient
		{
			DISALLOW_COPY_AND_ASSIGN(TinyHTTPClient)
		public:
			TinyHTTPClient(const string& strURL, USHORT sPORT);
			virtual ~TinyHTTPClient();
		protected:
			
			string		m_strURL;
			USHORT		m_sPORT;
		};
	}
}

