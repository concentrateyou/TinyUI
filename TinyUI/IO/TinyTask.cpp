#include "../stdafx.h"
#include <process.h>
#include "../Common/TinyUtility.h"
#include "../Common/TinyLogging.h"
#include "TinyTask.h"


namespace TinyUI
{
	namespace IO
	{
		TinyTask::TinyTask()
			:m_hTask(NULL)
		{
		}

		TinyTask::~TinyTask()
		{
			if (m_hTask != NULL)
			{
				CloseHandle(m_hTask);
				m_hTask = NULL;
			}
		}
		HANDLE	TinyTask::Handle() const
		{
			return m_hTask;
		}
		DWORD	TinyTask::GetTaskID() const
		{
			return m_dwTaskID;
		}
		BOOL TinyTask::SetPriority(DWORD dwPriority)
		{
			if (!m_hTask)
				return FALSE;
			return SetThreadPriority(m_hTask, dwPriority);
		}
		BOOL TinyTask::Submit(Closure&& callback)
		{
			m_callback = std::move(callback);
			if (m_hTask != NULL)
			{
				CloseHandle(m_hTask);
				m_hTask = NULL;
			}
			m_hTask = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TinyTask::Callback, (LPVOID)this, 0, &m_dwTaskID);
			return m_hTask != NULL;
		}
		BOOL TinyTask::Close(DWORD dwMS)
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

		BOOL TinyTask::IsActive() const
		{
			if (!m_hTask)
				return FALSE;
			DWORD code = 0;
			GetExitCodeThread(m_hTask, &code);
			return code == STILL_ACTIVE;
		}
		DWORD TinyTask::Suspend()
		{
			if (!m_hTask)
				return FALSE;
			return SuspendThread(m_hTask);
		}
		DWORD TinyTask::Resume()
		{
			if (!m_hTask)
				return FALSE;
			return ResumeThread(m_hTask);
		}
		BOOL TinyTask::Terminate(DWORD dwExit)
		{
			if (!m_hTask)
				return FALSE;
			return TerminateThread(m_hTask, dwExit);
		}
		DWORD WINAPI TinyTask::Callback(LPVOID ps)
		{
			TinyTask* pTask = reinterpret_cast<TinyTask*>(ps);
			if (pTask != NULL)
			{
				pTask->m_callback();
			}
			return FALSE;
		}
		//////////////////////////////////////////////////////////////////////////
		TinySimpleTaskTimer::TinySimpleTaskTimer()
			:m_delay(0),
			m_bBreak(FALSE)
		{

		}
		TinySimpleTaskTimer::~TinySimpleTaskTimer()
		{

		}
		BOOL TinySimpleTaskTimer::SetCallback(LONG delay, Closure&& callback)
		{
			m_delay = delay;
			m_callback = std::move(callback);
			m_bBreak = FALSE;
			return TRUE;
		}

		void TinySimpleTaskTimer::Close()
		{
			m_bBreak = TRUE;
			m_task.Close(INFINITE);
			LOG(INFO) << "TinyTaskTimer Close OK";
		}
		void TinySimpleTaskTimer::OnMessagePump()
		{
			for (;;)
			{
				if (m_bBreak)
					break;
				if (!m_callback.IsNull())
				{
					m_callback();
				}
				Sleep(m_delay);
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
				TranslateMessage(&msg);
				DispatchMessage(&msg);
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
