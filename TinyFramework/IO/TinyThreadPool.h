#pragma once
#include "../Common/TinyCollection.h"
#include "../Common/TinyCore.h"
#include "../Common/TinyCallback.h"
#include "../Common/TinyEvent.h"
#include "../IO/TinyWorker.h"

namespace TinyFramework
{
	namespace IO
	{
		/// <summary>
		/// ÈÎÎñ³Ø
		/// </summary>
		class TinyThreadpool
		{
			DISALLOW_COPY_AND_ASSIGN(TinyThreadpool)
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
			TinyThreadpool();
			BOOL PostTask(void*	context, Closure&& task, Closure&& complete);
		private:
			static DWORD CALLBACK TaskItemCallback(void* param);
		};
	};
}

