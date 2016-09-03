#include "../stdafx.h"
#include <process.h>
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
			if (m_hTask)
			{
				CloseHandle(m_hTask);
				m_hTask = NULL;
			}
		}

		BOOL TinyTask::Submit(Closure& callback)
		{
			m_callback = callback;
			m_hTask = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TinyTask::Callback, (LPVOID)this, 0, &m_dwTaskID);
			return m_hTask != NULL;
		}
		BOOL TinyTask::Wait(DWORD dwMs)
		{
			if (m_hTask)
			{
				switch (WaitForSingleObject(m_hTask, dwMs))
				{
				case WAIT_TIMEOUT:
					return FALSE;
				case WAIT_OBJECT_0:
					return TRUE;
				}
			}
			return TRUE;
		}

		BOOL TinyTask::IsValid() const
		{
			if (m_hTask)
			{
				DWORD code = 0;
				return GetExitCodeThread(m_hTask, &code) && code == STILL_ACTIVE;
			}
			return FALSE;
		}

		DWORD WINAPI TinyTask::Callback(LPVOID ps)
		{
			try
			{
				TinyTask* pTask = reinterpret_cast<TinyTask*>(ps);
				if (pTask)
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
