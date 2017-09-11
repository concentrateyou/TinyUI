#pragma once
#include "TinyHTTPCommon.h"
#include "TinyHTTPRequest.h"
#include "TinyURL.h"
#include "../IO/TinyRingBuffer.h"

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
			void	Close();
			LONG	Read(BYTE* ps, LONG size);
		private:
			TinyHTTPResponse(TinyHTTPRequest& request);
			BOOL ParseResponse(CHAR* s, INT size);
			BOOL ParseStatusLine(CHAR* ps1, CHAR* ps2);
			BOOL ParseAttribute(CHAR* ps1, CHAR* ps2);
			CHAR* ReadLine(CHAR* s);
		private:
			void OnHandleReceive(DWORD, AsyncResult*);
		private:
			CHAR				m_raw[DEFAULT_BUFFER_SIZE];
			DWORD				m_dwCode;
			string				m_desc;
			string				m_vs;
			string				m_body;
			TinyLock			m_lock;
			TinyHTTPRequest&	m_request;
			IO::TinyRingBuffer	m_buffer;
		};
	}
}

