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
				HRESULT hRes = WaitForSingleObjectEx(m_hTask, dwMS, TRUE);
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
		//////////////////////////////////////////////////////////////////////////
		TinyTaskTimer::TinyTaskTimer()
			:m_delay(0),
			m_bBreak(FALSE)
		{

		}
		TinyTaskTimer::~TinyTaskTimer()
		{

		}
		BOOL TinyTaskTimer::SetCallback(LONG delay, Closure&& callback)
		{
			m_delay = delay;
			m_callback = std::move(callback);
			if (m_timer.Create(TRUE, NULL))
			{
				m_bBreak = FALSE;
				return m_task.Submit(BindCallback(&TinyTaskTimer::OnMessagePump, this));
			}
			return FALSE;
		}
		void TinyTaskTimer::Close()
		{
			m_bBreak = TRUE;
			m_timer.Close();
			TRACE("TinyTaskTimer Close OK\n");
		}
		void TinyTaskTimer::OnMessagePump()
		{
			for (;;)
			{
				TRACE("TinyTaskTimer Begin Waiting\n");
				m_timer.Waiting(m_delay);
				TRACE("TinyTaskTimer End Waiting\n");
				if (!m_bBreak)
				{
					if (!m_callback.IsNull())
					{
						m_callback();
					}
				}
				if (m_bBreak)
				{
					TRACE("TinyTaskTimer Break = TRUE\n");
					break;
				}
				TRACE("TinyTaskTimer Continue\n");
			}
			TRACE("TinyTaskTimer OnMessagePump\n");
		}
	}
}
