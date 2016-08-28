#pragma once
#include "DXFramework.h"

namespace DXFramework
{
	class TaskRunner
	{
	public:
		TaskRunner();
		~TaskRunner();
		void Run(LPVOID ps);
	private:
		HANDLE	m_hTask;
	};
}

