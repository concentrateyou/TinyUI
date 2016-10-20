#pragma once
#include "DShowCommon.h"
#include "OutputPinBase.h"

namespace Media
{
	class SourceStream : public OutputPinBase
	{
	public:
		SourceStream(FilterBase* pFilter, WCHAR* pzName, TinyLock* lock);
		~SourceStream();
	};
}


