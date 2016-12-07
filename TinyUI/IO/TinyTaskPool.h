#pragma once
#include "TinyTaskBase.h"
#include "TinyThread.h"
#include <map>
#include <concurrent_queue.h>
#include <future>  

namespace TinyUI
{
	namespace IO
	{
		/// <summary>
		/// 延迟任务(创建并执行在给定延迟后启用的一次性操作)
		/// </summary>
		class DelayTask : public TaskItem
		{
			friend class TinyTaskPool;
		public:
			DelayTask(const Closure& callback, DWORD dwInitialDelay, DWORD dwTaskType = TASK_DELAY);
			virtual ~DelayTask();
			DWORD	GetInitialDelay() const;
			virtual void OnInvoke() OVERRIDE;
			virtual void OnComplete() OVERRIDE;
			virtual void OnDestory() OVERRIDE;
		protected:
			Closure	m_callback;
			DWORD	m_dwInitialDelay;
		};
		/// <summary>
		/// 计划任务(创建并执行一个在给定初始延迟后首次启用的定期操作,在每一次执行终止和下一次执行开始之间都存在给定的延迟)
		/// </summary>
		class ScheduleTask : public DelayTask
		{
			friend class TinyTaskPool;
		public:
			ScheduleTask(const Closure& callback, DWORD dwInitialDelay, DWORD dwDelay);
			virtual ~ScheduleTask();
			DWORD	GetDelay() const;
			BOOL	Wait();
		protected:
			Closure		m_callback;
			DWORD		m_dwDelay;
			TinyEvent	m_event;
		};
		/// <summary>
		/// 任务池
		/// </summary>
		class TinyTaskPool : public TaskRunner
		{
		public:
			TinyTaskPool(DWORD dwConcurrent);
			virtual ~TinyTaskPool();
		public:
			void PostTask(TaskItem* item) OVERRIDE;
			void PostQuit();
		private:
			void OnMessagePump();
		private:
			DWORD m_dwConcurrent;
			concurrency::concurrent_queue<TaskItem*> m_taskItems;
			TinyScopedArray<TinyTaskBase> m_tasks;
		};
	};
}

