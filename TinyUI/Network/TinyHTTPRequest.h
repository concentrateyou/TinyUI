#pragma once
#include "TinyHTTPMessage.h"
#include <map>

namespace TinyUI
{
	namespace Network
	{
		class TinyHTTPResponse;

		class TinyHTTPRequest : public TinyHTTPMessage
		{
			DISALLOW_COPY_AND_ASSIGN(TinyHTTPRequest)
		public:
			TinyHTTPRequest();
			BOOL Create(const string& szURL);
			void SetGeneral(const string& ms, const string& version);
			TinyHTTPResponse* GetResponse();
		private:
			string Build();
		private:
			IPEndPoint	m_endpoint;
			TinyURL		m_sURL;
			TinySocket	m_socket;
			string		m_version;
			string		m_ms;
		};
	}
}

