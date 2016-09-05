#pragma once
#include <Windows.h>
#include "Common/TinyCore.h"
using namespace TinyUI;

namespace Media
{
	class FilterObserver
	{
	public:
		FilterObserver();
		void Lock();
		void Unlock();
		virtual void OnFrameReceive(const BYTE* data, INT size, LPVOID lpParameter) = 0;
	protected:
		virtual ~FilterObserver(); 
	protected:
		TinyLock	m_lock;
	};
}
