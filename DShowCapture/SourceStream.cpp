#include "stdafx.h"
#include "SourceStream.h"

namespace Media
{
	SourceStream::SourceStream(FilterBase* pFilter, WCHAR* pzName, TinyLock* lock)
		:OutputPinBase(pFilter, pzName, lock)
	{

	}


	SourceStream::~SourceStream()
	{
	}
}
