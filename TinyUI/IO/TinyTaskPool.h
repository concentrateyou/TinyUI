#pragma once
#include "../Common/TinyCollection.h"
#include "../Common/TinyCore.h"
#include "../Common/TinyCallback.h"
#include "../Common/TinyEvent.h"
#include "../IO/TinyTask.h"

namespace TinyUI
{
	namespace IO
	{
		/// <summary>
		/// �����
		/// </summary>
		class TinyTaskPool
		{
			DISALLOW_COPY_AND_ASSIGN(TinyTaskPool)
		public:
			class TaskItem
			{
				DISALLOW_COPY_AND_ASSIGN(TaskItem)
			public:
				TaskItem(void*	context, Closure&& task, Closure&& complete);
				~TaskItem();
				void operator()();
			private:
				void*	m_context;
				Closure	m_task;
				Closure m_complete;
			};
		public:
			TinyTaskPool();
			BOOL PostTask(void*	context, Closure&& task, Closure&& complete);
		private:
			static DWORD CALLBACK TaskItemCallback(void* param);
		};
	};
}

