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
		/// <summary>
		/// 任务项
		/// </summary>
		class TaskItem
		{
		public:
			TaskItem();
			virtual ~TaskItem();
		public:
			virtual void Invoke() = 0;
			virtual void OnComplete() = 0;
		public:
			DWORD  GetDelay() const;
			void   SetDelay(DWORD dwDelay = 0);
			string GetName() const;
			void   SetName(const string& name);
		private:
			DWORD	m_dwDelay;
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

