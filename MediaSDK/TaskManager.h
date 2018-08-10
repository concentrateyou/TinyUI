#pragma once
#include "MediaSDK.h"

namespace MediaSDK
{
	class TaskManager
	{
		DISALLOW_COPY_AND_ASSIGN(TaskManager)
	public:
		TaskManager(DWORD dwMin, DWORD dwMax);
		~TaskManager();
	public:
		void PostTask();
	private:
		TinyWin32Threadpool				m_pool;
		TinyArray<TinyWin32Worker*>		m_works;
	};
}

