#pragma once
#include <Windows.h>
#include "Common/TinyCore.h"
using namespace TinyUI;

namespace Media
{
	class FilterObserver : public TinySRWLock
	{
		DECLARE_DYNAMIC(FilterObserver);
		DISALLOW_COPY_AND_ASSIGN(FilterObserver);
	public:
		FilterObserver();
		virtual void OnFrameReceive(const BYTE* data, INT size, LPVOID lpParameter) = 0;
	protected:
		virtual ~FilterObserver();
	};
}
