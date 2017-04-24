#pragma once
#include "TinyHTTPCommon.h"
#include "TinyURL.h"

namespace TinyUI
{
	namespace Network
	{
		class TinyHTTPRequest;

		class TinyHTTPResponse : public TinyHTTPAttribute
		{
			friend class TinyHTTPRequest;
			DISALLOW_COPY_AND_ASSIGN(TinyHTTPResponse)
		public:
			DWORD	GetCode() const;
			string	GetStatus() const;
			string	GetBody() const;	
			void	Close();
		private:
			TinyHTTPResponse();
			BOOL ParseResponse(CHAR* s, INT size);
			BOOL ParseStatusLine(CHAR* ps1, CHAR* ps2);
			BOOL ParseAttribute(CHAR* ps1, CHAR* ps2);
			CHAR* ReadLine(CHAR* s);
		private:
			DWORD	m_code;
			string	m_desc;
			string	m_vs;
			string	m_body;
		};
	}
}

