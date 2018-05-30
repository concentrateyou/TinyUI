#include "../stdafx.h"
#include <process.h>
#include "../Common/TinyUtility.h"
#include "../Common/TinyLogging.h"
#include "TinyProcess.h"


namespace TinyFramework
{
	namespace IO
	{
		TinyProcessPipe::TinyProcessPipe()
			:m_hRIO(NULL),
			m_hWIO(NULL)
		{

		}
		TinyProcessPipe::~TinyProcessPipe()
		{
			CloseInput();
			CloseOutput();
		}
		BOOL TinyProcessPipe::Create()
		{
			SECURITY_ATTRIBUTES attr = { 0 };
			attr.nLength = sizeof(attr);
			attr.lpSecurityDescriptor = NULL;
			attr.bInheritHandle = TRUE;
			if (!CreatePipe(&m_hRIO, &m_hWIO, &attr, 0))
				return FALSE;
			return TRUE;
		}
		void TinyProcessPipe::CloseInput()
		{
			if (m_hWIO != NULL)
			{
				CloseHandle(m_hWIO);
				m_hWIO = NULL;
			}
		}
		void TinyProcessPipe::CloseOutput()
		{
			if (m_hRIO != NULL)
			{
				CloseHandle(m_hRIO);
				m_hRIO = NULL;
			}
		}
		HANDLE TinyProcessPipe::GetInput() const
		{
			return m_hWIO;
		}
		HANDLE TinyProcessPipe::GetOutput() const
		{
			return m_hRIO;
		}
		DWORD TinyProcessPipe::Read(void* bits, DWORD size)
		{
			if (!m_hRIO ||
				m_hRIO == INVALID_HANDLE_VALUE
				|| !bits
				|| size == 0)
				return 0;
			DWORD dw = 0;
			::ReadFile(m_hRIO, bits, size, &dw, NULL);
			return dw;
		}
		DWORD TinyProcessPipe::Write(const void* bits, DWORD size)
		{
			if (!m_hWIO ||
				m_hWIO == INVALID_HANDLE_VALUE
				|| !bits
				|| size == 0)
				return 0;
			DWORD dw = 0;
			::WriteFile(m_hWIO, bits, size, &dw, NULL);
			return dw;
		}
		//////////////////////////////////////////////////////////////////////////
		TinyProcess::TinyProcess(HANDLE handle)
			: m_hProcess(handle)
		{
			m_handles[0] = m_handles[1] = m_handles[2] = NULL;
		}

		TinyProcess::TinyProcess(TinyProcess&& myO)
			: m_hProcess(myO.m_hProcess)
		{
			m_handles[0] = myO.m_handles[0];
			m_handles[1] = myO.m_handles[1];
			m_handles[2] = myO.m_handles[2];
			if (myO.m_hProcess != NULL)
			{
				::CloseHandle(myO.m_hProcess);
				myO.m_hProcess = NULL;
			}
		}

		TinyProcess::~TinyProcess()
		{
			if (m_hProcess != NULL)
			{
				::CloseHandle(m_hProcess);
				m_hProcess = NULL;
			}
		}

		TinyProcess& TinyProcess::operator=(TinyProcess&& myO)
		{
			m_hProcess = myO.m_hProcess;
			m_handles[0] = myO.m_handles[0];
			m_handles[1] = myO.m_handles[1];
			m_handles[2] = myO.m_handles[2];
			if (myO.m_hProcess != NULL)
			{
				::CloseHandle(myO.m_hProcess);
				myO.m_hProcess = NULL;
			}
			return *this;
		}
		void TinyProcess::SetInput(HANDLE handle)
		{
			m_handles[0] = handle;
		}
		void TinyProcess::SetOutput(HANDLE handle)
		{
			m_handles[1] = handle;
		}
		void TinyProcess::SetError(HANDLE handle)
		{
			m_handles[2] = handle;
		}
		BOOL TinyProcess::Open(DWORD dwPID)
		{
			m_hProcess = ::OpenProcess(PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION | SYNCHRONIZE, FALSE, dwPID);
			return m_hProcess != NULL;
		}
		BOOL TinyProcess::Create(const string& app, const vector<string>& args)
		{
			if (!PathFileExists(app.c_str()))
				return FALSE;
			STARTUPINFO si = {};
			si.cb = sizeof(STARTUPINFO);
			si.hStdInput = m_handles[0] == NULL ? GetStdHandle(STD_INPUT_HANDLE) : m_handles[0];
			si.hStdOutput = m_handles[1] == NULL ? GetStdHandle(STD_OUTPUT_HANDLE) : m_handles[1];
			si.hStdError = m_handles[2] == NULL ? GetStdHandle(STD_ERROR_HANDLE) : m_handles[2];
			si.dwFlags |= STARTF_USESTDHANDLES;
			PROCESS_INFORMATION info = {};
			string  line = app;
			for (size_t i = 0; i < args.size(); i++)
			{
				line.append(" ");
				line.append(args[i]);
			}
			if (!CreateProcess(app.c_str(), &line[0], NULL, NULL, TRUE, 0, NULL, NULL, &si, &info))
				return FALSE;
			m_hProcess = info.hProcess;
			return TRUE;
		}
		TinyProcess TinyProcess::Current()
		{
			return TinyProcess(GetCurrentProcess());
		}
		TinyProcess TinyProcess::Duplicate(HANDLE handle)
		{
			HANDLE hHandle = NULL;
			if (!::DuplicateHandle(GetCurrentProcess(), handle, GetCurrentProcess(), &hHandle, 0, FALSE, DUPLICATE_SAME_ACCESS))
			{
				return TinyProcess();
			}
			return TinyProcess(hHandle);
		}
		BOOL TinyProcess::IsEmpty() const
		{
			return m_hProcess == NULL || m_hProcess == INVALID_HANDLE_VALUE;
		}
		TinyProcess::operator HANDLE() const
		{
			return m_hProcess;
		}
		DWORD TinyProcess::GetPID() const
		{
			return GetProcessId(m_hProcess);
		}
		BOOL TinyProcess::Wait(DWORD dwMS, DWORD& dwExitCode)
		{
			if (::WaitForSingleObject(m_hProcess, dwMS) != WAIT_OBJECT_0)
				return FALSE;
			if (!::GetExitCodeProcess(m_hProcess, &dwExitCode))
				return FALSE;
			return TRUE;
		}
		BOOL TinyProcess::Terminate(DWORD dwMS)
		{
			BOOL bRes = TRUE;
			if (!::TerminateProcess(m_hProcess, 0))
			{
				bRes = FALSE;
			}
			if (dwMS > 0)
			{
				if (WaitForSingleObject(m_hProcess, dwMS) == WAIT_OBJECT_0)
				{
					bRes = TRUE;
				}
			}
			if (m_hProcess != NULL)
			{
				CloseHandle(m_hProcess);
				m_hProcess = NULL;
			}
			return bRes;
		}
		DWORD TinyProcess::GetPriority() const
		{
			return ::GetPriorityClass(m_hProcess);
		}
		BOOL TinyProcess::IsActive() const
		{
			if (!m_hProcess)
				return FALSE;
			DWORD dw = 0;
			if (!GetExitCodeProcess(m_hProcess, &dw) || dw != STILL_ACTIVE)
				return FALSE;
			return TRUE;
		}
	}
}
