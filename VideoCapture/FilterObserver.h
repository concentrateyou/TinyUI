#pragma once
#include <Windows.h>

namespace Media
{
	class FilterObserver
	{
	public:
		virtual void OnFrameReceive(const BYTE* data, INT size, LPVOID lpParameter) = 0;
	protected:
		virtual ~FilterObserver(); 
	};
}
