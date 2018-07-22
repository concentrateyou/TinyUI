#include "../stdafx.h"
#include <process.h>
#include "../Common/TinyUtility.h"
#include "../Common/TinyLogging.h"
#include "TinyThread.h"


namespace TinyFramework
{
	namespace IO
	{
		TinyThread::TinyThread()
			:m_handle(NULL)
		{

		}

		TinyThread::~TinyThread()
		{
			if (m_handle != NULL)
			{
				CloseHandle(m_handle);
				m_handle = NULL;
			}
		}
		HANDLE	TinyThread::Handle() const
		{
			return m_handle;
		}
		DWORD	TinyThread::GetID() const
		{
			return m_dwID;
		}
		BOOL TinyThread::SetPriority(DWORD dwPriority)
		{
			if (!m_handle)
				return FALSE;
			return SetThreadPriority(m_handle, dwPriority);
		}
		BOOL TinyThread::Submit(Closure&& callback)
		{
			m_callback = std::move(callback);
			if (m_handle != NULL)
			{
				CloseHandle(m_handle);
				m_handle = NULL;
			}
			m_handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TinyThread::Callback, (LPVOID)this, 0, &m_dwID);
			return m_handle != NULL;
		}
		BOOL TinyThread::Close(DWORD dwMS)
		{
			BOOL bRes = FALSE;
			if (m_handle != NULL)
			{
				HRESULT hRes = WaitForSingleObject(m_handle, dwMS);
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
			if (m_handle != NULL)
			{
				CloseHandle(m_handle);
				m_handle = NULL;
			}
			return bRes;
		}

		BOOL TinyThread::IsActive() const
		{
			if (!m_handle)
				return FALSE;
			DWORD code = 0;
			GetExitCodeThread(m_handle, &code);
			return code == STILL_ACTIVE;
		}
		DWORD TinyThread::Suspend()
		{
			if (!m_handle)
				return FALSE;
			return SuspendThread(m_handle);
		}
		DWORD TinyThread::Resume()
		{
			if (!m_handle)
				return FALSE;
			return ResumeThread(m_handle);
		}
		BOOL TinyThread::Terminate(DWORD dwExit)
		{
			if (!m_handle)
				return FALSE;
			return TerminateThread(m_handle, dwExit);
		}
		DWORD WINAPI TinyThread::Callback(LPVOID ps)
		{
			TinyThread* pTask = reinterpret_cast<TinyThread*>(ps);
			if (pTask != NULL)
			{
				pTask->m_callback();
			}
			return 0;
		}
		//////////////////////////////////////////////////////////////////////////
		TinyTLS::TinyTLS()
			:m_dwTlsIndex(0)
		{
			if ((m_dwTlsIndex = TlsAlloc()) == TLS_OUT_OF_INDEXES)
			{
				throw system_error(GetLastError(), system_category());
			}
		}
		void TinyTLS::SetValue(LPVOID ps)
		{
			TlsSetValue(m_dwTlsIndex, ps);
		}
		LPVOID TinyTLS::GetValue()
		{
			return TlsGetValue(m_dwTlsIndex);
		}
		TinyTLS::~TinyTLS()
		{
			TlsFree(m_dwTlsIndex);
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
			return m_runner.Submit(BindCallback(&TinySimpleTaskTimer::OnMessagePump, this));
		}

		void TinySimpleTaskTimer::Close()
		{
			m_bBreak = TRUE;
			m_runner.Close(INFINITE);
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
	}
}
