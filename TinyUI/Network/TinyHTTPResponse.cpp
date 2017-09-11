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
			m_buffer.Initialize(1024 * 1024 * 6, 1);
			if (!m_request.m_socket.BeginReceive(m_raw, DEFAULT_BUFFER_SIZE, 0, BindCallback(&TinyHTTPResponse::OnHandleReceive, this), this))
			{
				m_request.OnHandleError(GetLastError());
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
		void TinyHTTPResponse::OnHandleReceive(DWORD dwError, AsyncResult* result)
		{
			if (dwError != 0)
			{
				m_request.OnHandleError(dwError);
			}
			else
			{
				DWORD dwRes = m_request.m_socket.EndReceive(result);
				if (dwRes > 0)
				{
					TinyAutoLock lock(m_lock);
					m_buffer.Write(m_raw, dwRes);
					TRACE("Write:%d\n", dwRes);
					if (!m_request.m_socket.BeginReceive(m_raw, DEFAULT_BUFFER_SIZE, 0, BindCallback(&TinyHTTPResponse::OnHandleReceive, this), this))
					{
						m_request.OnHandleError(GetLastError());
					}
				}
			}
		}
		LONG TinyHTTPResponse::Read(BYTE* ps, LONG size)
		{
			TinyAutoLock lock(m_lock);
			LONG val = m_buffer.Read(ps, size);
			TRACE("read:%d\n", val);
			return val;
		}
	}
}