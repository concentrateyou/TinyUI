#include "../stdafx.h"
#include "TinyTaskPool.h"

namespace TinyUI
{
	namespace IO
	{
		TinyTaskPool::TaskItem::TaskItem(void* context, Closure&& task, Closure&& complete)
			:m_context(context),
			m_task(std::move(task)),
			m_complete(std::move(complete))
		{

		}
		TinyTaskPool::TaskItem::~TaskItem()
		{

		}
		void TinyTaskPool::TaskItem::operator()()
		{
			if (!m_task.IsNull())
			{
				m_task();
			}
			if (!m_complete.IsNull())
			{
				m_complete();
			}
		}
		//////////////////////////////////////////////////////////////////////////
		TinyTaskPool::TinyTaskPool()
		{

		}
		DWORD CALLBACK TinyTaskPool::TaskItemCallback(void* param)
		{
			TaskItem* ps = static_cast<TaskItem*>(param);
			(*ps)();
			SAFE_DELETE(ps);
			return 0;
		}
		BOOL TinyTaskPool::PostTask(void* context, Closure&& task, Closure&& complete)
		{
			TaskItem* ps = new TaskItem(context, std::move(task), std::move(complete));
			if (!QueueUserWorkItem(TaskItemCallback, ps, WT_EXECUTEDEFAULT))
			{
				SAFE_DELETE(ps);
				return FALSE;
			}
			return TRUE;
		}
	}
}
