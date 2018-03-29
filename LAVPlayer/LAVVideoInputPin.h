#pragma once
#include "DShowCommon.h"
#include "ScopedMediaType.h"
#include <dshow.h>
#include <uuids.h>
#include <string>
#include <list>
#include <vector>
#include <algorithm>
using namespace std;
using namespace TinyUI;
using namespace DShow;

namespace LAV
{
	class LAVVideoInputPin
	{
		DISALLOW_IMPLICIT_CONSTRUCTORS(LAVVideoInputPin)
	public:
		LAVVideoInputPin(FilterBase* pFilter, FilterObserver* observer);
		virtual ~LAVVideoInputPin();
		HRESULT CheckMediaType(const AM_MEDIA_TYPE* pMediaType) OVERRIDE;
		HRESULT GetMediaType(INT position, AM_MEDIA_TYPE* pMediaType) OVERRIDE;
	};
}

