#include "../stdafx.h"
#include <process.h>
#include "../Common/TinyUtility.h"
#include "TinyTaskBase.h"


namespace TinyUI
{
	namespace IO
	{
		TinyTaskBase::TinyTaskBase()
			:m_hTask(NULL)
		{
		}

		TinyTaskBase::~TinyTaskBase()
		{
			if (m_hTask != NULL)
			{
				CloseHandle(m_hTask);
				m_hTask = NULL;
			}
		}
		HANDLE	TinyTaskBase::Handle() const
		{
			return m_hTask;
		}
		BOOL TinyTaskBase::SetPriority(DWORD dwPriority)
		{
			if (!m_hTask)
				return FALSE;
			return SetThreadPriority(m_hTask, dwPriority);
		}
		BOOL TinyTaskBase::Submit(Closure&& callback)
		{
			m_callback = std::move(callback);
			if (m_hTask != NULL)
			{
				CloseHandle(m_hTask);
				m_hTask = NULL;
			}
			m_hTask = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TinyTaskBase::Callback, (LPVOID)this, 0, &m_dwTaskID);
			return m_hTask != NULL;
		}
		BOOL TinyTaskBase::Close(DWORD dwMS)
		{
			BOOL bRes = FALSE;
			if (m_hTask != NULL)
			{
				HRESULT hRes = WaitForSingleObject(m_hTask, dwMS);
				if (hRes == WAIT_TIMEOUT)
				{
					bRes = FALSE;
					goto _ERROR;
				}
				if (hRes == WAIT_OBJECT_0)
				{
					bRes = TRUE;
					goto _ERROR;
				}
			}
		_ERROR:
			if (m_hTask != NULL)
			{
				CloseHandle(m_hTask);
				m_hTask = NULL;
			}
			return bRes;
		}

		BOOL TinyTaskBase::IsActive() const
		{
			if (!m_hTask)
				return FALSE;
			DWORD code = 0;
			GetExitCodeThread(m_hTask, &code);
			return code == STILL_ACTIVE;
		}
		DWORD TinyTaskBase::Suspend()
		{
			if (!m_hTask)
				return FALSE;
			return SuspendThread(m_hTask);
		}
		DWORD TinyTaskBase::Resume()
		{
			if (!m_hTask)
				return FALSE;
			return ResumeThread(m_hTask);
		}
		BOOL TinyTaskBase::Terminate(DWORD dwExit)
		{
			if (!m_hTask)
				return FALSE;
			return TerminateThread(m_hTask, dwExit);
		}
		DWORD WINAPI TinyTaskBase::Callback(LPVOID ps)
		{
			try
			{
				TinyTaskBase* pTask = reinterpret_cast<TinyTaskBase*>(ps);
				if (pTask != NULL)
				{
					pTask->m_callback();
				}
			}
			catch (std::exception* e)
			{
				throw(e);
			}
			return FALSE;
		}
	}
}
