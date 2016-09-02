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
				DestroyThreadpoolEnvironment(&m_cbe);
				CloseThreadpool(m_pPool);
			}
			return FALSE;
		}
		PTP_WORK TinyWin32TaskPool::SubmitTask(PVOID pv, PTP_WORK_CALLBACK cb)
		{
			PTP_WORK ps = NULL;
			if ((ps = CreateThreadpoolWork(cb, pv, &m_cbe)) != NULL)
			{
				SubmitThreadpoolWork(ps);
			}
			return ps;
		}
		void TinyWin32TaskPool::WaitTask(PTP_WORK ps, BOOL fCancelPendingCallbacks)
		{
			WaitForThreadpoolWorkCallbacks(ps, fCancelPendingCallbacks);
		}
		void TinyWin32TaskPool::CloseTask(PTP_WORK ps)
		{
			CloseThreadpoolWork(ps);
		}
		void TinyWin32TaskPool::CancelPending()
		{
			CloseThreadpoolCleanupGroupMembers(m_pCleanup, TRUE, NULL);
		}
		void TinyWin32TaskPool::Close()
		{
			DestroyThreadpoolEnvironment(&m_cbe);
			CloseThreadpoolCleanupGroup(m_pCleanup);
			CloseThreadpool(m_pPool);
			m_pCleanup = NULL;
			m_pPool = NULL;
		}
		TinyWin32TaskPool::~TinyWin32TaskPool()
		{
			Close();
		}
		//////////////////////////////////////////////////////////////////////////
		TinyWin32Task::TinyWin32Task(TinyWin32TaskPool* pWorks)
			:m_pWorks(pWorks),
			m_work(NULL)
		{

		}
		TinyWin32Task::~TinyWin32Task()
		{

		}
		BOOL TinyWin32Task::Submit(Closure& callback)
		{
			if (m_pWorks)
			{
				m_callback = callback;
				m_work = m_pWorks->SubmitTask(this, TinyWin32Task::WorkCallback);
				return m_work != NULL;
			}
			return FALSE;
		}
		BOOL TinyWin32Task::Close()
		{
			if (m_pWorks && m_work)
			{
				m_pWorks->CloseTask(m_work);
				return TRUE;
			}
			return FALSE;
		}
		void TinyWin32Task::WorkCallback(PTP_CALLBACK_INSTANCE Instance, PVOID  Context, PTP_WORK  Work)
		{
			TinyWin32Task* pWork = reinterpret_cast<TinyWin32Task*>(Context);
			if (pWork->m_work == Work)
			{
				if (!pWork->m_callback.IsNull())
				{
					pWork->m_callback();
				}
			}
		}
	}
}
