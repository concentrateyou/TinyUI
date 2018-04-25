#include "../stdafx.h"
#include <process.h>
#include "../Common/TinyUtility.h"
#include "../Common/TinyLogging.h"
#include "TinyProcess.h"


namespace TinyUI
{
	namespace IO
	{
		TinyProcess::TinyProcess(HANDLE handle)
			: m_hProcess(handle)
		{
			m_handles[0] = m_handles[1] = m_handles[2] = NULL;
		}

		TinyProcess::TinyProcess(TinyProcess&& other)
			: m_hProcess(other.m_hProcess)
		{
			m_handles[0] = other.m_handles[0];
			m_handles[1] = other.m_handles[1];
			m_handles[2] = other.m_handles[2];
			if (other.m_hProcess != NULL)
			{
				::CloseHandle(other.m_hProcess);
				other.m_hProcess = NULL;
			}
		}

		TinyProcess::~TinyProcess()
		{
		}

		TinyProcess& TinyProcess::operator=(TinyProcess&& other)
		{
			m_hProcess = other.m_hProcess;
			m_handles[0] = other.m_handles[0];
			m_handles[1] = other.m_handles[1];
			m_handles[2] = other.m_handles[2];
			if (other.m_hProcess != NULL)
			{
				::CloseHandle(other.m_hProcess);
				other.m_hProcess = NULL;
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
		BOOL TinyProcess::Create(const string& szFile, string& szCMD)
		{
			if (!PathFileExists(szFile.c_str()))
				return FALSE;
			STARTUPINFO si = {};
			si.cb = sizeof(STARTUPINFO);
			si.hStdInput = m_handles[0] == NULL ? GetStdHandle(STD_INPUT_HANDLE) : m_handles[0];
			si.hStdOutput = m_handles[1] == NULL ? GetStdHandle(STD_OUTPUT_HANDLE) : m_handles[1];
			si.hStdError = m_handles[2] == NULL ? GetStdHandle(STD_ERROR_HANDLE) : m_handles[2];
			si.dwFlags |= STARTF_USESTDHANDLES;
			PROCESS_INFORMATION info = {};
			if (!CreateProcess(szFile.c_str(), &szCMD[0], NULL, NULL, TRUE, 0, NULL, NULL, &si, &info))
			{
				return FALSE;
			}
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
		BOOL TinyProcess::Close(DWORD dwMS)
		{
			if (::WaitForSingleObject(m_hProcess, dwMS) != WAIT_OBJECT_0)
				return FALSE;
			DWORD dwExitCode = 0;
			if (!::GetExitCodeProcess(m_hProcess, &dwExitCode))
				return FALSE;
			return TRUE;
		}
		BOOL TinyProcess::Terminate(UINT uExitCode)
		{
			if (!::TerminateProcess(m_hProcess, uExitCode))
				return FALSE;
			return TRUE;
		}
		BOOL TinyProcess::WaitForTerminate(UINT uExitCode, DWORD dwMS)
		{
			if (!::TerminateProcess(m_hProcess, uExitCode))
				return FALSE;
			return WaitForSingleObject(m_hProcess, dwMS) == WAIT_OBJECT_0;
		}
		INT TinyProcess::GetPriority() const
		{
			return ::GetPriorityClass(m_hProcess);
		}
	}
}
