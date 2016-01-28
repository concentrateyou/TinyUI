#include "../stdafx.h"
#include "vld.h"
#include <process.h>
#include "TinyTaskPool.h"

namespace TinyUI
{
	namespace IO
	{
		TinyTaskPool::TinyTaskPool()
			:m_pPool(NULL),
			m_pCleanup(NULL)
		{
			memset(&m_cbe, 0, sizeof(m_cbe));
		}
		BOOL TinyTaskPool::Initialize(DWORD dwMin, DWORD dwMax)
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
		PTP_WORK TinyTaskPool::SubmitTask(PVOID pv, PTP_WORK_CALLBACK cb)
		{
			PTP_WORK ps = NULL;
			if ((ps = CreateThreadpoolWork(cb, pv, &m_cbe)) != NULL)
			{
				SubmitThreadpoolWork(ps);
			}
			return ps;
		}
		void TinyTaskPool::WaitTask(PTP_WORK ps, BOOL fCancelPendingCallbacks)
		{
			WaitForThreadpoolWorkCallbacks(ps, fCancelPendingCallbacks);
		}
		void TinyTaskPool::CloseTask(PTP_WORK ps)
		{
			CloseThreadpoolWork(ps);
		}
		void TinyTaskPool::CancelPending()
		{
			CloseThreadpoolCleanupGroupMembers(m_pCleanup, TRUE, NULL);
		}
		void TinyTaskPool::Close()
		{
			DestroyThreadpoolEnvironment(&m_cbe);
			CloseThreadpoolCleanupGroup(m_pCleanup);
			CloseThreadpool(m_pPool);
			m_pCleanup = NULL;
			m_pPool = NULL;
		}
		TinyTaskPool::~TinyTaskPool()
		{
			Close();
		}
	}
}
