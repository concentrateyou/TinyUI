#include "../stdafx.h"
#include "TinyTaskPool.h"

namespace TinyUI
{
	namespace IO
	{
		TinyTaskPool::TinyTaskPool(DWORD dwConcurrent)
			:m_dwConcurrent(dwConcurrent)
		{
			m_tasks.Reset(new TinyTaskBase[m_dwConcurrent]);
			for (DWORD i = 0;i < dwConcurrent;i++)
			{
				m_tasks[i].Submit(BindCallback(&TinyTaskPool::MessagePump, this));
			}
		}
		TinyTaskPool::~TinyTaskPool()
		{

		}
		void TinyTaskPool::MessagePump()
		{
			for (;;)
			{
				TaskItem* item = NULL;
				if (m_taskItems.try_pop(item) && item)
				{
					Sleep(item->GetDelay());
					item->Invoke();
					item->OnComplete();
				}
			}
		}
		void TinyTaskPool::PostTask(TaskItem* item)
		{
			m_taskItems.push(item);
		}
	}
}
