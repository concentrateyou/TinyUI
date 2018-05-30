#pragma once
#include <Windows.h>
#include "Common/TinyCore.h"
using namespace TinyFramework;

namespace MF
{
	class MFObserver : public TinyLock
	{
		DISALLOW_COPY_AND_ASSIGN(MFObserver)
	public:
		MFObserver();
		virtual void OnFrameReceive(BYTE* bits, LONG size, FLOAT ts, LPVOID lpParameter) = 0;
	protected:
		virtual ~MFObserver();
	};

}


