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
		/// �ӳ�����(������ִ���ڸ����ӳٺ����õ�һ���Բ���)
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
		/// �ƻ�����(������ִ��һ���ڸ�����ʼ�ӳٺ��״����õĶ��ڲ���,��ÿһ��ִ����ֹ����һ��ִ�п�ʼ֮�䶼���ڸ������ӳ�)
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
		/// �����
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

