#include "../stdafx.h"
#include "TinyTaskPool.h"

namespace TinyUI
{
	namespace IO
	{
		DelayTask::DelayTask(const Closure& callback, DWORD dwInitialDelay, DWORD dwTaskType)
			:TaskItem(dwTaskType),
			m_dwInitialDelay(dwInitialDelay),
			m_callback(std::move(callback))
		{

		}
		DelayTask::~DelayTask()
		{

		}
		DWORD DelayTask::GetInitialDelay() const
		{
			return m_dwInitialDelay;
		}
		void DelayTask::OnInvoke()
		{
			
		}
		void DelayTask::OnComplete()
		{
			
		}
		void DelayTask::OnDestory()
		{
			
		}
		//////////////////////////////////////////////////////////////////////////
		ScheduleTask::ScheduleTask(const Closure& callback, DWORD dwInitialDelay, DWORD dwDelay)
			:DelayTask(callback, dwInitialDelay, TASK_SCHEDULE),
			m_dwDelay(dwDelay)
		{
			m_event.CreateEvent();
		}
		ScheduleTask::~ScheduleTask()
		{

		}
		DWORD ScheduleTask::GetDelay() const
		{
			return m_dwDelay;
		}
		BOOL ScheduleTask::Wait()
		{
			m_event.Lock(m_dwDelay);
			return TRUE;
		}
		//////////////////////////////////////////////////////////////////////////
		TinyTaskPool::TinyTaskPool(DWORD dwConcurrent)
			: m_dwConcurrent(dwConcurrent)
		{
			m_tasks.Reset(new TinyTaskBase[m_dwConcurrent]);
			for (DWORD i = 0;i < dwConcurrent;i++)
			{
				m_tasks[i].Submit(BindCallback(&TinyTaskPool::OnMessagePump, this));
			}
		}
		TinyTaskPool::~TinyTaskPool()
		{

		}
		void TinyTaskPool::OnMessagePump()
		{ 
			for (;;)
			{
				TaskItem* item = NULL;
				if (m_taskItems.try_pop(item) && item)
				{
					if (item->GetTaskType() == TaskItem::TASK_QUIT)
					{
						break;
					}
					switch (item->GetTaskType())
					{
					case TaskItem::TASK_DELAY:
					{
						if (DelayTask* s = static_cast<DelayTask*>(item))
						{
							Sleep(s->GetInitialDelay());
							s->OnInvoke();
							if (!s->m_callback.IsNull())
							{
								s->m_callback();
							}
						}
					}
					break;
					case TaskItem::TASK_SCHEDULE:
					{
						if (ScheduleTask* s = static_cast<ScheduleTask*>(item))
						{
							Sleep(s->GetInitialDelay());
							s->OnInvoke();
							if (!s->m_callback.IsNull())
							{
								s->m_callback();
							}
						}
					}
					break;
					}
					item->OnDestory();
				}
			}
		}
		void TinyTaskPool::PostTask(TaskItem* item)
		{
			m_taskItems.push(item);
		}

		void TinyTaskPool::PostQuit()
		{

		}
	}
}
