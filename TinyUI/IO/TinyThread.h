#pragma once
#include "../Common/TinyCollection.h"
#include "../Common/TinyCore.h"
#include "../Common/TinyCallback.h"
#include "../Common/TinyEvent.h"
#include <string>
#include <algorithm>
using namespace std;

namespace TinyUI
{
	namespace IO
	{
		class TaskItem
		{
			friend class TinyTaskPool;
		public:
			enum
			{
				TASK_UNKNOW,
				TASK_DELAY,
				TASK_SCHEDULE,
				TASK_QUIT
			};
		public:
			TaskItem(DWORD dwTaskType = TASK_UNKNOW);
			virtual ~TaskItem();
		public:
			virtual void OnInvoke() = 0;
			virtual void OnComplete() = 0;
			virtual void OnDestory() = 0;
		public:
			DWORD	GetTaskType() const;
			string	GetName() const;
			void	SetName(const string& name);
		protected:
			DWORD 	m_dwTaskType;
			string	m_name;
		};
		/// <summary>
		/// Task投递任务 
		/// </summary>
		class TaskRunner
		{
		protected:
			TaskRunner();
			virtual ~TaskRunner();
		public:
			virtual void PostTask(TaskItem* item) = 0;
		};
		/// <summary>
		/// 线程本地存储
		/// </summary>
		class TinyTLS
		{
			DISALLOW_COPY_AND_ASSIGN(TinyTLS)
		public:
			TinyTLS();
			~TinyTLS();
			void	SetValue(LPVOID ps);
			LPVOID	GetValue();
		private:
			DWORD m_dwTlsIndex;
		};
	};
}

