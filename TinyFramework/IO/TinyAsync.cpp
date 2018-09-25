#include "../stdafx.h"
#include "TinyAsync.h"

namespace TinyFramework
{
	namespace IO
	{
		TinyWorkerPoolTaskRunner::TinyTask::TinyTask()
			:m_worker(NULL),
			m_waiter(NULL),
			m_event(NULL),
			m_delay(0)
		{

		}
		TinyWorkerPoolTaskRunner::TinyTask::~TinyTask()
		{
			SAFE_DELETE(m_waiter);
			SAFE_DELETE(m_worker);
			SAFE_CLOSE_HANDLE(m_event);
		}
		BOOL TinyWorkerPoolTaskRunner::TinyTask::Execute()
		{
			BOOL bRes = FALSE;
			if (m_delay > 0)
			{
				if (m_waiter != NULL)
				{
					m_event = CreateEvent(NULL, FALSE, FALSE, NULL);
					bRes = m_waiter->Submit(m_event, m_delay, BindCallback(&TinyTask::OnTask, this));
				}
			}
			else
			{
				if (m_worker != NULL)
				{
					bRes = m_worker->Submit(BindCallback(&TinyTask::OnTask, this));
				}
			}
			return bRes;
		}
		void TinyWorkerPoolTaskRunner::TinyTask::OnTask()
		{
			if (!m_callback.IsNull())
			{
				m_callback();
			}
			SAFE_CLOSE_HANDLE(m_event);
			if (m_waiter != NULL)
				m_waiter->Close();
			if (m_worker != NULL)
				m_worker->Close();
			delete this;
		}
		//////////////////////////////////////////////////////////////////////////
		TinyWorkerPoolTaskRunner::TinyWorkerPoolTaskRunner(INT iMax)
		{
			m_pool.Initialize(1, iMax);
		}
		TinyWorkerPoolTaskRunner::~TinyWorkerPoolTaskRunner()
		{
			m_pool.Cancel();
			m_pool.Close();
		}
		BOOL TinyWorkerPoolTaskRunner::PostTask(Closure&& callback, INT delay)
		{
			if (delay > 0)
			{
				TinyTask* task = new TinyTask();
				if (!task)
				{
					TRACE("[TinyWorkerPoolTaskRunner] Create TinyTask FAIL\n");
					return FALSE;
				}
				task->m_waiter = new TinyWin32Waiter(&m_pool);
				ASSERT(task->m_waiter);
				task->m_callback = std::move(callback);
				if (!task->Execute())
				{
					SAFE_DELETE(task);
					return FALSE;
				}
			}
			else
			{
				TinyTask* task = new TinyTask();
				if (!task)
				{
					TRACE("[TinyWorkerPoolTaskRunner] Create TinyTask FAIL\n");
					return FALSE;
				}
				task->m_worker = new TinyWin32Worker(&m_pool);
				ASSERT(task->m_worker);
				task->m_callback = std::move(callback);
				if (!task->Execute())
				{
					SAFE_DELETE(task);
					return FALSE;
				}
			}
			return TRUE;
		}
		//////////////////////////////////////////////////////////////////////////
		TinyTaskManeger::TinyTask::TinyTask()
			:m_event(NULL),
			m_delay(0)
		{

		}
		TinyTaskManeger::TinyTask::~TinyTask()
		{
			m_waiter = NULL;
			m_worker = NULL;
			SAFE_CLOSE_HANDLE(m_event);
		}
		void TinyTaskManeger::TinyTask::Close()
		{
			if (m_waiter != NULL)
			{
				m_waiter->Wait(FALSE);
				m_waiter->Close();
			}
			m_waiter = NULL;
			if (m_worker != NULL)
			{
				m_worker->Wait(FALSE);
				m_worker->Close();
			}
			m_worker = NULL;
		}
		BOOL TinyTaskManeger::TinyTask::Execute()
		{
			BOOL bRes = FALSE;
			if (m_delay > 0)
			{
				if (m_waiter != NULL)
				{
					m_event = CreateEvent(NULL, FALSE, FALSE, NULL);
					bRes = m_waiter->Submit(m_event, m_delay, BindCallback(&TinyTask::OnTask, this));
				}
			}
			else
			{
				if (m_worker != NULL)
				{
					bRes = m_worker->Submit(BindCallback(&TinyTask::OnTask, this));
				}
			}
			return bRes;
		}
		void TinyTaskManeger::TinyTask::OnTask()
		{
			if (!m_callback.IsNull())
			{
				m_callback();
			}
			SAFE_CLOSE_HANDLE(m_event);
		}

		TinyTaskManeger::TinyTaskManeger(INT iMax)
		{
			m_pool.Initialize(1, iMax);
		}
		TinyTaskManeger::~TinyTaskManeger()
		{
			m_pool.Cancel();
			m_pool.Close();
		}

		TinyScopedReferencePtr<TinyTaskManeger::TinyTask> TinyTaskManeger::PostTask(Closure&& callback, INT delay)
		{
			TinyScopedReferencePtr<TinyTaskManeger::TinyTask> task(new TinyTask());
			ASSERT(task);
			if (delay > 0)
			{
				task->m_waiter = new TinyWin32Waiter(&m_pool);
				task->m_callback = std::move(callback);
				if (!task->Execute())
				{
					task = NULL;
					return task;
				}
			}
			else
			{
				task->m_worker = new TinyWin32Worker(&m_pool);
				task->m_callback = std::move(callback);
				if (!task->Execute())
				{
					task = NULL;
					return task;
				}
			}
			return task;
		}
	}
}
