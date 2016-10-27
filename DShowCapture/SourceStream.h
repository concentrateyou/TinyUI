#pragma once
#include "DShowCommon.h"
#include "OutputPinBase.h"

namespace DShow
{
	class SourceStream : public OutputPinBase
	{
	public:
		SourceStream(FilterBase* pFilter, WCHAR* pzName, TinyLock* lock);
		~SourceStream();
	};
}


