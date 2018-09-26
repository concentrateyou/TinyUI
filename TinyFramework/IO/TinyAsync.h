#pragma once
#include "../Common/TinyCollection.h"
#include "../Common/TinyCore.h"
#include "../Common/TinyCallback.h"
#include "../Common/TinyEvent.h"
#include "../IO/TinyThreadpool.h"
using namespace TinyFramework;

namespace TinyFramework
{
	namespace IO
	{
		class TinyAsync
		{
			DISALLOW_COPY_AND_ASSIGN(TinyAsync)
		private:
			class TaskItem
			{
				DISALLOW_COPY_AND_ASSIGN(TaskItem)
			public:
				TaskItem(void*	context, Closure&& callback, Closure&& complete);
				~TaskItem();
				void operator()();
			private:
				void*	m_context;
				Closure	m_callback;
				Closure m_complete;
			};
		public:
			TinyAsync();
			BOOL PostTask(void*	context, Closure&& callback, Closure&& complete);
		private:
			static DWORD CALLBACK TaskItemCallback(void* param);
		};
	};
}

