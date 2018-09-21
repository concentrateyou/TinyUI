#include "../stdafx.h"
#include <process.h>
#include "../Common/TinyUtility.h"
#include "../Common/TinyLogging.h"
#include "TinyWorker.h"


namespace TinyFramework
{
	namespace IO
	{
		TinyWorker::TinyWorker()
			:m_handle(NULL)
		{

		}

		TinyWorker::~TinyWorker()
		{
			SAFE_CLOSE_HANDLE(m_handle);
		}
		HANDLE	TinyWorker::Handle() const
		{
			return m_handle;
		}
		DWORD	TinyWorker::GetID() const
		{
			return m_dwID;
		}
		INT TinyWorker::GetPriority() const
		{
			return ::GetThreadPriority(m_handle);
		}
		BOOL TinyWorker::SetPriority(INT priority)
		{
			return ::SetThreadPriority(m_handle, priority);
		}
		BOOL TinyWorker::Submit(Closure&& callback)
		{
			m_callback = std::move(callback);
			SAFE_CLOSE_HANDLE(m_handle);
			m_handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TinyWorker::Callback, (LPVOID)this, 0, &m_dwID);
			return m_handle != NULL;
		}
		BOOL TinyWorker::Close(DWORD dwMS)
		{
			BOOL bRes = FALSE;
			if (m_handle != NULL)
			{
				bRes == (WaitForSingleObject(m_handle, dwMS) == WAIT_OBJECT_0);
			}
			SAFE_CLOSE_HANDLE(m_handle);
			return bRes;
		}

		BOOL TinyWorker::IsActive() const
		{
			if (!m_handle)
				return FALSE;
			DWORD code = 0;
			::GetExitCodeThread(m_handle, &code);
			return code == STILL_ACTIVE;
		}
		DWORD TinyWorker::Suspend()
		{
			if (!m_handle)
				return FALSE;
			return ::SuspendThread(m_handle);
		}
		DWORD TinyWorker::Resume()
		{
			if (!m_handle)
				return FALSE;
			return ::ResumeThread(m_handle);
		}
		BOOL TinyWorker::Terminate(DWORD dwExit)
		{
			if (!m_handle)
				return FALSE;
			return ::TerminateThread(m_handle, dwExit);
		}
		DWORD WINAPI TinyWorker::Callback(LPVOID ps)
		{
			TinyWorker* pTask = reinterpret_cast<TinyWorker*>(ps);
			if (pTask != NULL)
			{
				pTask->SetPriority(THREAD_PRIORITY_NORMAL);
				pTask->m_callback();
			}
			return 1;
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
