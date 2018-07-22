#include "../stdafx.h"
#include <process.h>
#include "TinyWin32Threadpool.h"

namespace TinyFramework
{
	namespace IO
	{
		TinyWin32Threadpool::TinyWin32Threadpool()
			:m_pPool(NULL),
			m_pCleanup(NULL)
		{
			memset(&m_cbe, 0, sizeof(m_cbe));
		}
		TinyWin32Threadpool::~TinyWin32Threadpool()
		{
			Close();
		}
		BOOL TinyWin32Threadpool::Initialize(DWORD dwMin, DWORD dwMax)
		{
			InitializeThreadpoolEnvironment(&m_cbe);
			if ((m_pPool = CreateThreadpool(NULL)) != NULL)
			{
				SetThreadpoolThreadMinimum(m_pPool, dwMin);
				SetThreadpoolThreadMaximum(m_pPool, dwMax);
				SetThreadpoolCallbackPool(&m_cbe, m_pPool);
				if ((m_pCleanup = CreateThreadpoolCleanupGroup()) != NULL)
				{
					SetThreadpoolCallbackCleanupGroup(&m_cbe, m_pCleanup, NULL);
					return TRUE;
				}
			}
			return FALSE;
		}

		void TinyWin32Threadpool::Cancel(BOOL fCancelPendingCallbacks)
		{
			if (m_pCleanup != NULL)
			{
				CloseThreadpoolCleanupGroupMembers(m_pCleanup, fCancelPendingCallbacks, NULL);
			}
		}
		void TinyWin32Threadpool::Close()
		{
			if (m_pCleanup != NULL)
			{
				CloseThreadpoolCleanupGroupMembers(m_pCleanup, FALSE, NULL);
				CloseThreadpoolCleanupGroup(m_pCleanup);
				m_pCleanup = NULL;
			}
			if (m_pPool != NULL)
			{
				DestroyThreadpoolEnvironment(&m_cbe);
				CloseThreadpool(m_pPool);
				m_pPool = NULL;
			}
		}
		//////////////////////////////////////////////////////////////////////////
		TinyWin32Worker::TinyWin32Worker(TinyWin32Threadpool* Threadpool)
			:m_work(NULL),
			m_cbe(NULL)
		{
			m_cbe = (Threadpool != NULL) ? &Threadpool->m_cbe : NULL;
		}
		TinyWin32Worker::~TinyWin32Worker()
		{
			Close();
		}
		BOOL TinyWin32Worker::Submit(Closure&& callback)
		{
			if (m_work != NULL)
			{
				SubmitThreadpoolWork(m_work);
				return TRUE;
			}
			else
			{
				m_callback = std::move(callback);
				m_work = CreateThreadpoolWork(TinyWin32Worker::WorkCallback, this, m_cbe);
				if (m_work != NULL)
				{
					SubmitThreadpoolWork(m_work);
				}
				return m_work != NULL;
			}
			return FALSE;
		}
		BOOL TinyWin32Worker::Wait(BOOL fCancelPendingCallbacks)
		{
			if (m_work != NULL)
			{
				WaitForThreadpoolWorkCallbacks(m_work, fCancelPendingCallbacks);
				return TRUE;
			}
			return FALSE;
		}
		void TinyWin32Worker::Close()
		{
			if (m_work != NULL)
			{
				CloseThreadpoolWork(m_work);
				m_work = NULL;
			}
		}
		void TinyWin32Worker::WorkCallback(PTP_CALLBACK_INSTANCE Instance, PVOID  Context, PTP_WORK  Work)
		{
			TinyWin32Worker* pThis = reinterpret_cast<TinyWin32Worker*>(Context);
			if (pThis->m_work == Work)
			{
				if (!pThis->m_callback.IsNull())
				{
					pThis->m_callback();
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////
		TinyWin32Timer::TinyWin32Timer(TinyWin32Threadpool* pTaskPool)
			:m_timer(NULL),
			m_cbe(NULL)
		{
			m_cbe = (pTaskPool != NULL) ? &pTaskPool->m_cbe : NULL;
		}
		TinyWin32Timer::~TinyWin32Timer()
		{
			Close();
		}
		BOOL TinyWin32Timer::Submit(DWORD msDelay, DWORD msPeriod, Closure&& callback)
		{
			FILETIME ftime;
			ULARGE_INTEGER time;
			time.QuadPart = static_cast<ULONGLONG>(-static_cast<LONGLONG>(msDelay)* (1 * 10 * 1000));
			ftime.dwHighDateTime = time.HighPart;
			ftime.dwLowDateTime = time.LowPart;
			if (m_timer != NULL)
			{
				SetThreadpoolTimer(m_timer, &ftime, msPeriod, 0);
				return TRUE;
			}
			else
			{
				m_callback = std::move(callback);
				m_timer = CreateThreadpoolTimer(TinyWin32Timer::TimerCallback, this, m_cbe);
				if (m_timer != NULL)
				{
					SetThreadpoolTimer(m_timer, &ftime, msPeriod, 0);
				}
				return m_timer != NULL;
			}
			return FALSE;
		}
		BOOL TinyWin32Timer::Wait(BOOL fCancelPendingCallbacks)
		{
			if (m_timer != NULL)
			{
				WaitForThreadpoolTimerCallbacks(m_timer, fCancelPendingCallbacks);
				return TRUE;
			}
			return FALSE;
		}
		void TinyWin32Timer::Close()
		{
			if (m_timer != NULL)
			{
				CloseThreadpoolTimer(m_timer);
				m_timer = NULL;
			}
		}
		void NTAPI TinyWin32Timer::TimerCallback(PTP_CALLBACK_INSTANCE Instance, PVOID  Context, PTP_TIMER Timer)
		{
			TinyWin32Timer* pThis = reinterpret_cast<TinyWin32Timer*>(Context);
			if (pThis->m_timer == Timer)
			{
				if (!pThis->m_callback.IsNull())
				{
					pThis->m_callback();
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////
		TinyWin32Waiter::TinyWin32Waiter(TinyWin32Threadpool* pTaskPool)
			:m_wait(NULL),
			m_cbe(NULL),
			m_event()
		{
			m_cbe = (pTaskPool != NULL) ? &pTaskPool->m_cbe : NULL;
		}
		TinyWin32Waiter::~TinyWin32Waiter()
		{
			Close();
		}
		BOOL TinyWin32Waiter::Submit(HANDLE handle, DWORD msDelay, Closure&& callback)
		{
			FILETIME ftime;
			ULARGE_INTEGER time;
			time.QuadPart = static_cast<ULONGLONG>(-static_cast<LONGLONG>(msDelay)* (1 * 10 * 1000));
			ftime.dwHighDateTime = time.HighPart;
			ftime.dwLowDateTime = time.LowPart;
			if (m_wait != NULL)
			{
				SetThreadpoolWait(m_wait, handle, &ftime);
				return TRUE;
			}
			else
			{
				m_callback = std::move(callback);
				m_wait = CreateThreadpoolWait(TinyWin32Waiter::WaitCallback, this, m_cbe);
				if (m_wait != NULL)
				{
					SetThreadpoolWait(m_wait, handle, &ftime);
				}
				return m_wait != NULL;
			}
			return FALSE;
		}
		BOOL TinyWin32Waiter::Wait(BOOL fCancelPendingCallbacks)
		{
			if (m_wait != NULL)
			{
				WaitForThreadpoolWaitCallbacks(m_wait, fCancelPendingCallbacks);
				return TRUE;
			}
			return FALSE;
		}
		void TinyWin32Waiter::Close()
		{
			if (m_wait != NULL)
			{
				CloseThreadpoolWait(m_wait);
				m_wait = NULL;
			}
		}
		void NTAPI TinyWin32Waiter::WaitCallback(PTP_CALLBACK_INSTANCE Instance, PVOID  Context, PTP_WAIT Wait, TP_WAIT_RESULT WaitResult)
		{
			TinyWin32Waiter* pThis = reinterpret_cast<TinyWin32Waiter*>(Context);
			if (pThis->m_wait == Wait)
			{
				if (!pThis->m_callback.IsNull())
				{
					pThis->m_callback();
				}
			}
		}
	}
}
