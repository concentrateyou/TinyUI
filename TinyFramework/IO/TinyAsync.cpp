#include "../stdafx.h"
#include "TinyAsync.h"

namespace TinyFramework
{
	namespace IO
	{
		TinyAsync::TaskItem::TaskItem(void* context, Closure&& task, Closure&& complete)
			:m_context(context),
			m_task(std::move(task)),
			m_complete(std::move(complete))
		{

		}
		TinyAsync::TaskItem::~TaskItem()
		{

		}
		void TinyAsync::TaskItem::operator()()
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
		TinyAsync::TinyAsync()
		{

		}
		DWORD CALLBACK TinyAsync::TaskItemCallback(void* param)
		{
			TaskItem* ps = static_cast<TaskItem*>(param);
			(*ps)();
			SAFE_DELETE(ps);
			return 0;
		}
		BOOL TinyAsync::PostTask(void* context, Closure&& task, Closure&& complete)
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
