#include "stdafx.h"
#include "TaskManager.h"

namespace MediaSDK
{
	TaskManager::TaskManager(DWORD dwMin, DWORD dwMax)
	{
		m_pool.Initialize(dwMin, dwMax);
	}

	TaskManager::~TaskManager()
	{
		for (INT i = 0; i < m_works.GetSize(); i++)
		{
			m_works[i]->Close();
		}
		m_pool.Cancel(1);
		m_pool.Close();
	}
	void TaskManager::PostTask()
	{
		TinyWin32Worker* worker = new TinyWin32Worker(&m_pool);
		m_works.Add(worker);
		worker->Close();
	}
}