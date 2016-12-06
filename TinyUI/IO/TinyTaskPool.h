#pragma once
#include "TinyTaskBase.h"
#include "TinyThread.h"
#include <map>
#include <concurrent_queue.h>

namespace TinyUI
{
	namespace IO
	{
		/// <summary>
		/// ÈÎÎñ³Ø
		/// </summary>
		class TinyTaskPool : public TaskRunner
		{
		public:
			TinyTaskPool(DWORD dwConcurrent);
			virtual ~TinyTaskPool();
		public:
			void PostTask(TaskItem* item) OVERRIDE;
		private:
			void MessagePump();
		private:
			DWORD m_dwConcurrent;
			concurrency::concurrent_queue<TaskItem*> m_taskItems;
			TinyScopedArray<TinyTaskBase> m_tasks;
		};
	};
}

