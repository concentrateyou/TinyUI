#include "../stdafx.h"
#include <process.h>
#include "../Common/TinyUtility.h"
#include "../Common/TinyLogging.h"
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
		DWORD	TinyTaskBase::GetTaskID() const
		{
			return m_dwTaskID;
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
			if (m_timer.Create(NULL))
			{
				m_bBreak = FALSE;
				if (m_task.Submit(BindCallback(&TinyTaskTimer::OnMessagePump, this)))
				{
					return m_timer.SetWaiting(delay);
				}
			}
			return FALSE;
		}

		void TinyTaskTimer::Close()
		{
			m_bBreak = TRUE;
			m_timer.Close();
			m_task.Close(INFINITE);
			LOG(INFO) << "TinyTaskTimer Close OK";
		}
		void TinyTaskTimer::OnMessagePump()
		{
			for (;;)
			{
				if (m_bBreak)
					break;
				if (m_timer.Waiting())
				{
					if (!m_callback.IsNull())
					{
						m_callback();
					}
					m_timer.SetWaiting(m_delay);
				}
			}
			TRACE("EXIT\n");
			LOG(INFO) << "TinyTaskTimer EXIT";
		}
		//////////////////////////////////////////////////////////////////////////
		TinyMsgQueue::TinyMsgQueue()
		{

		}
		TinyMsgQueue::~TinyMsgQueue()
		{

		}
		BOOL TinyMsgQueue::SetCallback(Callback<void(UINT, WPARAM, LPARAM)>&& callback)
		{
			m_callback = std::move(callback);
			return m_task.Submit(BindCallback(&TinyMsgQueue::OnMessagePump, this));
		}
		BOOL TinyMsgQueue::Close()
		{
			PostThreadMessage(m_task.GetTaskID(), WM_MSGQUEUE_EXIT, NULL, NULL);
			return m_task.Close(INFINITE);
		}
		BOOL TinyMsgQueue::PostMsg(MSG& msg)
		{
			return PostThreadMessage(m_task.GetTaskID(), msg.message, msg.wParam, msg.lParam);
		}
		void TinyMsgQueue::OnMessagePump()
		{
			MSG msg = { 0 };
			for (;;)
			{
				if (!GetMessage(&msg, NULL, 0, 0))
					break;
				if (msg.message == WM_MSGQUEUE_EXIT)
					break;
				if (!m_callback.IsNull())
				{
					m_callback(msg.message, msg.wParam, msg.lParam);
				}
			}
		}
	}
}
