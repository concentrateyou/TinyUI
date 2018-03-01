#include "../stdafx.h"
#include <process.h>
#include "TinyPipe.h"
#include <atomic>

namespace TinyUI
{
	namespace IO
	{
		PIPE_IO_CONTEXT::PIPE_IO_CONTEXT()
		{
			Reset();
		}
		PIPE_IO_CONTEXT::~PIPE_IO_CONTEXT()
		{
		}
		void PIPE_IO_CONTEXT::Reset()
		{
			this->Internal = 0;
			this->InternalHigh = 0;
			this->Offset = 0;
			this->OffsetHigh = 0;
			this->Pointer = NULL;
			this->hEvent = INVALID_HANDLE_VALUE;
		}
		//////////////////////////////////////////////////////////////////////////
		TinyPipe::TinyPipe()
		{
			m_handles[0] = NULL;
			m_handles[1] = NULL;
		}

		TinyPipe::~TinyPipe()
		{

		}

		void TinyPipe::Close()
		{
			if (m_handles[0] != NULL)
			{
				CloseHandle(m_handles[0]);
			}
			if (m_handles[1] != NULL)
			{
				CloseHandle(m_handles[1]);
			}
			m_handles[0] = NULL;
			m_handles[1] = NULL;
		}
		//////////////////////////////////////////////////////////////////////////
		TinyPipeClient::TinyPipeClient()
			:m_hFile(NULL)
		{
			ZeroMemory(&m_context, sizeof(m_context));
		}
		TinyPipeClient::~TinyPipeClient()
		{

		}
		BOOL TinyPipeClient::Open(LPCSTR lpszPipename)
		{
			ASSERT(lpszPipename);
			m_szPipename = lpszPipename;
			return TRUE;
		}

		BOOL TinyPipeClient::IsEmpty() const
		{
			return (m_hFile == NULL || m_hFile == INVALID_HANDLE_VALUE);
		}

		BOOL TinyPipeClient::Connect()
		{
			while (1)
			{
				m_hFile = CreateFile(m_szPipename.c_str(),
					GENERIC_READ | GENERIC_WRITE,
					0,
					NULL,
					OPEN_EXISTING,
					SECURITY_SQOS_PRESENT | SECURITY_ANONYMOUS |
					FILE_FLAG_OVERLAPPED,
					NULL);
				if (m_hFile == INVALID_HANDLE_VALUE)
				{
					if (GetLastError() != ERROR_PIPE_BUSY)
						return FALSE;
					if (!WaitNamedPipe(m_szPipename.c_str(), 1000))//Ä¬ÈÏµÈ´ý1s
						return FALSE;
				}
				break;
			}
			//BindIoCompletionCallback(m_hFile, );
			if (ConnectNamedPipe(m_hFile, &m_context))
				return FALSE;
			DWORD dwError = GetLastError();
			switch (dwError)
			{
			case ERROR_IO_PENDING:
				break;
			case ERROR_PIPE_CONNECTED:
				break;
			default:
				return FALSE;
			}
			return TRUE;
		}

		void TinyPipeClient::Close()
		{
			if (!IsEmpty())
			{
				CloseHandle(m_hFile);
				m_hFile = NULL;
			}
		}
	}
}
