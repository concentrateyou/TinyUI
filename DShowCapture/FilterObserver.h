#pragma once
#include <Windows.h>
#include "Common/TinyCore.h"
using namespace TinyUI;

namespace DShow
{
	class InputPinBase;

	class FilterObserver : public TinyLock
	{
		friend class InputPinBase;
		DISALLOW_COPY_AND_ASSIGN(FilterObserver)
	public:
		FilterObserver();
		virtual void OnFrameReceive(BYTE* bits, LONG size, FLOAT ts, LPVOID lpParameter) = 0;
	protected:
		virtual ~FilterObserver();
	};
}
