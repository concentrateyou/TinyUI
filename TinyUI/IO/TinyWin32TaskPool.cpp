#include "../stdafx.h"
#include <process.h>
#include "TinyWin32TaskPool.h"

namespace TinyUI
{
	namespace IO
	{
		TinyWin32TaskPool::TinyWin32TaskPool()
			:m_pPool(NULL),
			m_pCleanup(NULL)
		{
			memset(&m_cbe, 0, sizeof(m_cbe));
		}
		BOOL TinyWin32TaskPool::Initialize(DWORD dwMin, DWORD dwMax)
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
		PTP_WORK TinyWin32TaskPool::SubmitTask(PVOID pv, PTP_WORK_CALLBACK cb)
		{
			PTP_WORK pWork = NULL;
			if ((pWork = CreateThreadpoolWork(cb, pv, &m_cbe)) != NULL)
			{
				SubmitThreadpoolWork(pWork);
			}
			return pWork;
		}

		void TinyWin32TaskPool::WaitTask(PTP_WORK pWork, BOOL fCancelPendingCallbacks)
		{
			if (pWork != NULL)
			{
				WaitForThreadpoolWorkCallbacks(pWork, fCancelPendingCallbacks);
			}
		}
		void TinyWin32TaskPool::CloseTask(PTP_WORK pWork)
		{
			if (pWork != NULL)
			{
				CloseThreadpoolWork(pWork);
				pWork = NULL;
			}
		}
		PTP_TIMER TinyWin32TaskPool::SubmitTimer(PVOID pv, PTP_TIMER_CALLBACK cb, DWORD msDelay, DWORD msPeriod)
		{
			PTP_TIMER pTimer = NULL;
			if ((pTimer = CreateThreadpoolTimer(cb, pv, &m_cbe)) != NULL)
			{
				FILETIME fileDueTime;
				ULARGE_INTEGER ulDueTime;
				ulDueTime.QuadPart = static_cast<ULONGLONG>(-static_cast<LONGLONG>(msDelay)* (1 * 10 * 1000));
				fileDueTime.dwHighDateTime = ulDueTime.HighPart;
				fileDueTime.dwLowDateTime = ulDueTime.LowPart;
				SetThreadpoolTimer(pTimer, &fileDueTime, msPeriod, 0);
			}
			return pTimer;
		}
		void TinyWin32TaskPool::WaitTimer(PTP_TIMER pTimer, BOOL fCancelPendingCallbacks)
		{
			if (pTimer != NULL)
			{
				WaitForThreadpoolTimerCallbacks(pTimer, fCancelPendingCallbacks);
			}
		}
		void TinyWin32TaskPool::SetTimer(PTP_TIMER pTimer, DWORD msDelay, DWORD msPeriod)
		{
			if (pTimer != NULL)
			{
				FILETIME fileDueTime;
				ULARGE_INTEGER ulDueTime;
				ulDueTime.QuadPart = static_cast<ULONGLONG>(-static_cast<LONGLONG>(msDelay)* (1 * 10 * 1000));
				fileDueTime.dwHighDateTime = ulDueTime.HighPart;
				fileDueTime.dwLowDateTime = ulDueTime.LowPart;
				SetThreadpoolTimer(pTimer, &fileDueTime, msPeriod, 0);
			}
		}
		void TinyWin32TaskPool::CloseTimer(PTP_TIMER pTimer)
		{
			if (pTimer != NULL)
			{
				CloseThreadpoolTimer(pTimer);
				pTimer = NULL;
			}
		}
		PTP_WAIT TinyWin32TaskPool::SubmitWaitItem(PVOID pv, PTP_WAIT_CALLBACK cb, HANDLE handle, DWORD msDelay)
		{
			PTP_WAIT pWaitItem = NULL;
			if ((pWaitItem = CreateThreadpoolWait(cb, pv, &m_cbe)) != NULL)
			{
				if (msDelay != INFINITE)
				{
					FILETIME timeout;
					ULARGE_INTEGER time;
					time.QuadPart = static_cast<ULONGLONG>(-static_cast<LONGLONG>(msDelay)* (1 * 10 * 1000));
					timeout.dwHighDateTime = time.HighPart;
					timeout.dwLowDateTime = time.LowPart;
					SetThreadpoolWait(pWaitItem, handle, &timeout);
				}
				else
				{
					SetThreadpoolWait(pWaitItem, handle, NULL);
				}
			}
			return pWaitItem;
		}
		void TinyWin32TaskPool::SetWaitItem(PTP_WAIT pWaitItem, HANDLE handle, DWORD msDelay)
		{
			if (pWaitItem != NULL)
			{
				if (msDelay != INFINITE)
				{
					FILETIME timeout;
					ULARGE_INTEGER time;
					time.QuadPart = static_cast<ULONGLONG>(-static_cast<LONGLONG>(msDelay)* (1 * 10 * 1000));
					timeout.dwHighDateTime = time.HighPart;
					timeout.dwLowDateTime = time.LowPart;
					SetThreadpoolWait(pWaitItem, handle, &timeout);
				}
				else
				{
					SetThreadpoolWait(pWaitItem, handle, NULL);
				}
			}
		}
		void TinyWin32TaskPool::WaitForWaitItem(PTP_WAIT pWaitItem, BOOL fCancelPendingCallbacks)
		{
			if (pWaitItem != NULL)
			{
				WaitForThreadpoolWaitCallbacks(pWaitItem, fCancelPendingCallbacks);
			}
		}
		void TinyWin32TaskPool::CloseWaitItem(PTP_WAIT pWaitItem)
		{
			if (pWaitItem != NULL)
			{
				CloseThreadpoolWait(pWaitItem);
				pWaitItem = NULL;
			}
		}
		void TinyWin32TaskPool::CancelPending()
		{
			if (m_pCleanup != NULL)
			{
				CloseThreadpoolCleanupGroupMembers(m_pCleanup, TRUE, NULL);
			}
		}
		void TinyWin32TaskPool::Close()
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
		TinyWin32TaskPool::~TinyWin32TaskPool()
		{
			Close();
		}
		//////////////////////////////////////////////////////////////////////////
		TinyWin32Task::TinyWin32Task(TinyWin32TaskPool* pTaskPool)
			:m_pTaskPool(pTaskPool),
			m_work(NULL)
		{

		}
		TinyWin32Task::~TinyWin32Task()
		{
			Close();
		}
		BOOL TinyWin32Task::Submit(Closure&& callback)
		{
			if (m_pTaskPool != NULL)
			{
				if (m_work != NULL)
				{
					SubmitThreadpoolWork(m_work);
					return TRUE;
				}
				else
				{
					m_callback = std::move(callback);
					m_work = m_pTaskPool->SubmitTask(this, TinyWin32Task::WorkCallback);
					return m_work != NULL;
				}

			}
			return FALSE;
		}
		BOOL TinyWin32Task::Wait(BOOL fCancelPendingCallbacks)
		{
			if (m_pTaskPool != NULL && m_work != NULL)
			{
				m_pTaskPool->WaitTask(m_work, fCancelPendingCallbacks);
				return TRUE;
			}
			return FALSE;
		}
		BOOL TinyWin32Task::Close()
		{
			if (m_pTaskPool != NULL && m_work != NULL)
			{
				m_pTaskPool->CloseTask(m_work);
				m_work = NULL;
				return TRUE;
			}
			return FALSE;
		}
		void TinyWin32Task::WorkCallback(PTP_CALLBACK_INSTANCE Instance, PVOID  Context, PTP_WORK  Work)
		{
			TinyWin32Task* pThis = reinterpret_cast<TinyWin32Task*>(Context);
			if (pThis->m_work == Work)
			{
				if (!pThis->m_callback.IsNull())
				{
					pThis->m_callback();
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////
		TinyWin32Timer::TinyWin32Timer(TinyWin32TaskPool* pTaskPool)
			:m_pTaskPool(pTaskPool),
			m_timer(NULL)
		{

		}
		TinyWin32Timer::~TinyWin32Timer()
		{
			Close();
		}
		BOOL TinyWin32Timer::Submit(DWORD msDelay, DWORD msPeriod, Closure&& callback)
		{
			if (m_pTaskPool != NULL)
			{
				if (m_timer != NULL)
				{
					m_pTaskPool->SetTimer(m_timer, msDelay, msPeriod);
					return TRUE;
				}
				else
				{
					m_callback = std::move(callback);
					m_timer = m_pTaskPool->SubmitTimer(this, TinyWin32Timer::TimerCallback, msDelay, msPeriod);
					return m_timer != NULL;
				}
			}
			return FALSE;
		}
		BOOL TinyWin32Timer::Wait(BOOL fCancelPendingCallbacks)
		{
			if (m_pTaskPool != NULL && m_timer != NULL)
			{
				m_pTaskPool->WaitTimer(m_timer, fCancelPendingCallbacks);
				return TRUE;
			}
			return FALSE;
		}
		BOOL TinyWin32Timer::Close()
		{
			if (m_pTaskPool != NULL && m_timer != NULL)
			{
				m_pTaskPool->CloseTimer(m_timer);
				m_timer = NULL;
				return TRUE;
			}
			return FALSE;
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
		TinyWin32Waiter::TinyWin32Waiter(TinyWin32TaskPool* pTaskPool)
			:m_pTaskPool(pTaskPool),
			m_waitItem(NULL)
		{

		}
		TinyWin32Waiter::~TinyWin32Waiter()
		{
			Close();
		}
		BOOL TinyWin32Waiter::Submit(HANDLE handle, DWORD dwDelay, Closure&& callback)
		{
			if (m_pTaskPool != NULL)
			{
				if (m_waitItem != NULL)
				{
					m_pTaskPool->SetWaitItem(m_waitItem, handle, dwDelay);
					return TRUE;
				}
				else
				{
					m_callback = std::move(callback);
					m_waitItem = m_pTaskPool->SubmitWaitItem(this, TinyWin32Waiter::WaitCallback, handle, dwDelay);
					return m_waitItem != NULL;
				}
			}
			return FALSE;
		}
		BOOL TinyWin32Waiter::Close()
		{
			if (m_pTaskPool != NULL && m_waitItem != NULL)
			{
				m_pTaskPool->CloseWaitItem(m_waitItem);
				m_waitItem = NULL;
				return TRUE;
			}
			return FALSE;
		}
		void NTAPI TinyWin32Waiter::WaitCallback(PTP_CALLBACK_INSTANCE Instance, PVOID  Context, PTP_WAIT Wait, TP_WAIT_RESULT WaitResult)
		{
			TinyWin32Waiter* pThis = reinterpret_cast<TinyWin32Waiter*>(Context);
			if (pThis->m_waitItem == Wait)
			{
				if (!pThis->m_callback.IsNull())
				{
					pThis->m_callback();
				}
			}
		}
	}
}
