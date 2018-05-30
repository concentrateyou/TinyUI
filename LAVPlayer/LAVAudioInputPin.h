#pragma once
#include "DShowCommon.h"
#include "ScopedMediaType.h"
#include "FilterBase.h"
#include "FilterObserver.h"
#include "InputPinBase.h"
#include <dshow.h>
#include <uuids.h>
#include <string>
#include <list>
#include <vector>
#include <algorithm>
using namespace std;
using namespace TinyFramework;
using namespace DShow;

namespace LAV
{
	class LAVAudio;

	class LAVAudioInputPin : public InputPinBase
	{
		DISALLOW_IMPLICIT_CONSTRUCTORS(LAVAudioInputPin)
	public:
		LAVAudioInputPin(FilterBase* pFilter, FilterObserver* observer);
		virtual ~LAVAudioInputPin();
		HRESULT CheckMediaType(const AM_MEDIA_TYPE* pMediaType) OVERRIDE;
		HRESULT GetMediaType(INT position, AM_MEDIA_TYPE* pMediaType) OVERRIDE;
	};
}



