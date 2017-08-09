#include "../stdafx.h"
#include "TinyHTTPResponse.h"
#include "TinyHTTPRequest.h"
#include <process.h>

namespace TinyUI
{
	namespace Network
	{
		TinyHTTPResponse::TinyHTTPResponse(TinyHTTPRequest& request)
			:m_request(request)
		{

		}

		DWORD TinyHTTPResponse::GetCode() const
		{
			return m_dwCode;
		}

		string	TinyHTTPResponse::GetStatus() const
		{
			return m_desc;
		}

		string	TinyHTTPResponse::GetBody() const
		{
			return m_body;
		}

		void TinyHTTPResponse::Close()
		{
			delete this;
		}

		BOOL TinyHTTPResponse::ParseResponse(CHAR* s, INT size)
		{
			CHAR* line = ReadLine(s);
			if (!ParseStatusLine(s, line - 2))
				return FALSE;
			while (line != NULL)
			{
				CHAR* ps = line;
				line = ReadLine(ps);
				if (*line == '\r' && *(line + 1) == '\n')
					break;
				if (!ParseAttribute(ps, line - 2))
					return FALSE;
			}
			return TRUE;
		}

		BOOL TinyHTTPResponse::ParseStatusLine(CHAR* ps1, CHAR* ps2)
		{
			if (!ps1 || !ps2)
				return FALSE;
			if (strncasecmp(ps1, TinyHTTPRequest::HTTP10, 8) != 0 &&
				strncasecmp(ps1, TinyHTTPRequest::HTTP11, 8) != 0 &&
				strncasecmp(ps1, TinyHTTPRequest::HTTP20, 8) != 0)
				return FALSE;
			m_vs = string(ps1, 8);
			ps1 += 8;
			while (*ps1 == ' ')
				++ps1;
			CHAR* ps3 = ps1;
			while (IsAsciiDigit(*ps3))
				++ps3;
			string val(ps1, ps3 - ps1);
			m_dwCode = atoi(val.c_str());
			while (*ps3 == ' ')
				++ps3;
			m_desc.append(ps3, ps2);
			return TRUE;
		}

		BOOL TinyHTTPResponse::ParseAttribute(CHAR* ps1, CHAR* ps2)
		{
			if (!ps1 || !ps2)
				return FALSE;
			CHAR* ps3 = ps1;
			while (*ps3 != ':')
				++ps3;
			CHAR* ps4 = ps3;
			while (*ps4 != ' ')
				++ps4;
			Add(string(ps1, ps3 - ps1), string(ps4, ps2 - ps4));
			return TRUE;
		}

		CHAR* TinyHTTPResponse::ReadLine(CHAR* s)
		{
			CHAR* val = s;
			while (*val++ != '\r');
			if (*val++ == '\n')
				return val;
			return NULL;
		}
	}
}