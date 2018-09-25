#include "../stdafx.h"
#include <process.h>
#include "TinyThreadpool.h"

namespace TinyFramework
{
	namespace IO
	{
		TinyThreadpool::TinyThreadpool()
			:m_pPool(NULL),
			m_pCleanup(NULL)
		{
			ZeroMemory(&m_cbe, sizeof(m_cbe));
		}
		TinyThreadpool::~TinyThreadpool()
		{
			Close();
		}
		BOOL TinyThreadpool::Initialize(DWORD dwMin, DWORD dwMax)
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

		void TinyThreadpool::Cancel(BOOL fCancelPendingCallbacks)
		{
			if (m_pCleanup != NULL)
			{
				CloseThreadpoolCleanupGroupMembers(m_pCleanup, fCancelPendingCallbacks, NULL);
			}
		}
		void TinyThreadpool::Close()
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
		TinyThreadpoolWorker::TinyThreadpoolWorker(TinyThreadpool& pool)
			:m_work(NULL),
			m_pool(pool)
		{

		}
		TinyThreadpoolWorker::~TinyThreadpoolWorker()
		{
			Close();
		}
		BOOL TinyThreadpoolWorker::Submit(Closure&& callback)
		{
			if (m_work != NULL)
			{
				SubmitThreadpoolWork(m_work);
				return TRUE;
			}
			else
			{
				m_callback = std::move(callback);
				m_work = CreateThreadpoolWork(TinyThreadpoolWorker::WorkCallback, this, &m_pool.m_cbe);
				if (m_work != NULL)
				{
					SubmitThreadpoolWork(m_work);
				}
				return m_work != NULL;
			}
			return FALSE;
		}
		BOOL TinyThreadpoolWorker::Wait(BOOL fCancelPendingCallbacks)
		{
			if (m_work != NULL)
			{
				WaitForThreadpoolWorkCallbacks(m_work, fCancelPendingCallbacks);
				return TRUE;
			}
			return FALSE;
		}
		void TinyThreadpoolWorker::Close()
		{
			if (m_work != NULL)
			{
				CloseThreadpoolWork(m_work);
				m_work = NULL;
			}
		}
		void TinyThreadpoolWorker::WorkCallback(PTP_CALLBACK_INSTANCE Instance, PVOID  Context, PTP_WORK  Work)
		{
			TinyThreadpoolWorker* pThis = reinterpret_cast<TinyThreadpoolWorker*>(Context);
			if (pThis->m_work == Work)
			{
				if (!pThis->m_callback.IsNull())
				{
					pThis->m_callback();
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////
		TinyThreadpoolTimer::TinyThreadpoolTimer(TinyThreadpool& pool)
			:m_timer(NULL),
			m_pool(pool)
		{

		}
		TinyThreadpoolTimer::~TinyThreadpoolTimer()
		{
			Close();
		}
		BOOL TinyThreadpoolTimer::Submit(DWORD msDelay, DWORD msPeriod, Closure&& callback)
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
				m_timer = CreateThreadpoolTimer(TinyThreadpoolTimer::TimerCallback, this, &m_pool.m_cbe);
				if (m_timer != NULL)
				{
					SetThreadpoolTimer(m_timer, &ftime, msPeriod, 0);
				}
				return m_timer != NULL;
			}
			return FALSE;
		}
		BOOL TinyThreadpoolTimer::Wait(BOOL fCancelPendingCallbacks)
		{
			if (m_timer != NULL)
			{
				WaitForThreadpoolTimerCallbacks(m_timer, fCancelPendingCallbacks);
				return TRUE;
			}
			return FALSE;
		}
		void TinyThreadpoolTimer::Close()
		{
			if (m_timer != NULL)
			{
				CloseThreadpoolTimer(m_timer);
				m_timer = NULL;
			}
		}
		void NTAPI TinyThreadpoolTimer::TimerCallback(PTP_CALLBACK_INSTANCE Instance, PVOID  Context, PTP_TIMER Timer)
		{
			TinyThreadpoolTimer* pThis = reinterpret_cast<TinyThreadpoolTimer*>(Context);
			if (pThis->m_timer == Timer)
			{
				if (!pThis->m_callback.IsNull())
				{
					pThis->m_callback();
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////
		TinyThreadpoolWaiter::TinyThreadpoolWaiter(TinyThreadpool& pool)
			:m_wait(NULL),
			m_pool(pool)
		{

		}
		TinyThreadpoolWaiter::~TinyThreadpoolWaiter()
		{
			Close();
		}
		BOOL TinyThreadpoolWaiter::Submit(HANDLE handle, DWORD msDelay, Closure&& callback)
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
				m_wait = CreateThreadpoolWait(TinyThreadpoolWaiter::WaitCallback, this, &m_pool.m_cbe);
				if (m_wait != NULL)
				{
					SetThreadpoolWait(m_wait, handle, &ftime);
				}
				return m_wait != NULL;
			}
			return FALSE;
		}
		BOOL TinyThreadpoolWaiter::Wait(BOOL fCancelPendingCallbacks)
		{
			if (m_wait != NULL)
			{
				WaitForThreadpoolWaitCallbacks(m_wait, fCancelPendingCallbacks);
				return TRUE;
			}
			return FALSE;
		}
		void TinyThreadpoolWaiter::Close()
		{
			if (m_wait != NULL)
			{
				CloseThreadpoolWait(m_wait);
				m_wait = NULL;
			}
		}
		void NTAPI TinyThreadpoolWaiter::WaitCallback(PTP_CALLBACK_INSTANCE Instance, PVOID  Context, PTP_WAIT Wait, TP_WAIT_RESULT WaitResult)
		{
			TinyThreadpoolWaiter* pThis = reinterpret_cast<TinyThreadpoolWaiter*>(Context);
			if (pThis->m_wait == Wait)
			{
				if (!pThis->m_callback.IsNull())
				{
					pThis->m_callback();
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////
		void TaskRunnerTraits::Destruct(const TinyTaskRunner* runner)
		{
			runner->OnDestruct();
		}
		TinyTaskRunner::TinyTaskRunner()
		{

		}
		TinyTaskRunner::~TinyTaskRunner()
		{

		}
		void TinyTaskRunner::OnDestruct() const
		{
			delete this;
		}
		//////////////////////////////////////////////////////////////////////////
		TinyTask::TinyTask(Closure&& callback, INT32 delay)
			:m_callback(std::move(callback)),
			m_delay(delay)
		{

		}
		TinyTask::~TinyTask()
		{

		}
		void TinyTask::operator()()
		{
			if (!m_callback.IsNull())
				m_callback();
		}
		INT32 TinyTask::delay() const
		{
			return m_delay;
		}
		//////////////////////////////////////////////////////////////////////////
		TinyThreadpoolTask::TinyThreadpoolTask(Closure&& callback, INT32 delay, TinyThreadpool& pool)
			:TinyTask(std::move(callback), delay),
			m_worker(pool),
			m_waiter(pool),
			m_handle(NULL)
		{

		}
		TinyThreadpoolTask::~TinyThreadpoolTask()
		{
			if (m_delay > 0)
			{
				m_waiter.Close();
				SAFE_CLOSE_HANDLE(m_handle);
			}
			else
			{
				m_worker.Close();
			}
		}
		BOOL TinyThreadpoolTask::Invoke()
		{
			if (m_delay > 0)
			{
				m_handle = CreateEvent(NULL, FALSE, FALSE, NULL);
				return m_waiter.Submit(m_handle, m_delay, BindCallback(&TinyThreadpoolTask::OnInvoke, this));
			}
			else
			{
				return m_worker.Submit(BindCallback(&TinyThreadpoolTask::OnInvoke, this));
			}
			return FALSE;
		}
		void TinyThreadpoolTask::OnInvoke()
		{
			if (!m_callback.IsNull())
			{
				m_callback();
			}
		}
		void TinyThreadpoolTask::Close()
		{
			if (m_delay > 0)
			{
				m_waiter.Wait(TRUE);
				m_waiter.Close();
				SAFE_CLOSE_HANDLE(m_handle);
			}
			else
			{
				m_worker.Wait(TRUE);
				m_worker.Close();
			}
		}
		//////////////////////////////////////////////////////////////////////////
		TinyThreadpoolTaskRunner::TinyThreadpoolTaskRunner(INT32 iMax)
		{
			m_pool.Initialize(1, iMax);
		}
		TinyThreadpoolTaskRunner::~TinyThreadpoolTaskRunner()
		{
			m_pool.Cancel();
			m_pool.Close();
		}
		TinyScopedReferencePtr<TinyTask> TinyThreadpoolTaskRunner::CreateTask(Closure&& callback, INT32 delay)
		{
			TinyThreadpoolTask* task = new TinyThreadpoolTask(std::move(callback), delay, m_pool);
			return task;
		}
		BOOL TinyThreadpoolTaskRunner::PostTask(TinyTask* task)
		{
			if (task != NULL)
				return task->Invoke();
			return FALSE;
		}
		BOOL TinyThreadpoolTaskRunner::PostTask(Closure&& callback, INT32 delay)
		{
			TinyThreadpoolTask* task = new TinyThreadpoolTask(std::move(callback), delay, m_pool);
			if (task != NULL)
				return task->Invoke();
			return FALSE;
		}
		//////////////////////////////////////////////////////////////////////////
		TaskScheduler::TaskScheduler()
		{

		}
		TaskScheduler::~TaskScheduler()
		{

		}
		TinyScopedReferencePtr<TinyTaskRunner> TaskScheduler::CreateTaskRunner()
		{
			TinyThreadpoolTaskRunner* runner = new TinyThreadpoolTaskRunner(12);
			return runner;
		}
	}
}
