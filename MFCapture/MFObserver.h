#pragma once
#include <Windows.h>
#include "Common/TinyCore.h"
using namespace TinyUI;

namespace MF
{
	class MFObserver : public TinyLock
	{
		DECLARE_DYNAMIC(MFObserver)
		DISALLOW_COPY_AND_ASSIGN(MFObserver)
	public:
		MFObserver();
		virtual void OnFrameReceive(BYTE* bits, LONG size, FLOAT ts, LPVOID lpParameter) = 0;
		virtual void OnError(HRESULT hRes);
	protected:
		virtual ~MFObserver();
	};

}


