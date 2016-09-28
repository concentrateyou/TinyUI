#pragma once
#include <Windows.h>
#include "Common/TinyCore.h"
using namespace TinyUI;

namespace Media
{
	class InputPinBase;

	class FilterObserver
	{
		friend class InputPinBase;
		DECLARE_DYNAMIC(FilterObserver);
		DISALLOW_COPY_AND_ASSIGN(FilterObserver);
	public:
		FilterObserver();
		virtual void OnFrameReceive(BYTE* bits, LONG size, LPVOID lpParameter) = 0;
	protected:
		virtual ~FilterObserver();
	};
}
