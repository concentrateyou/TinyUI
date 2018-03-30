#pragma once
#include "DShowCommon.h"
#include "ScopedMediaType.h"
#include "FilterBase.h"
#include "FilterObserver.h"
#include "LAVAudioInputPin.h"
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
	class __declspec(uuid("CD1606B2-2173-4C02-90D9-7A526C0F24D8"))
		LAVAudioFilter : public FilterBase
	{
		DISALLOW_IMPLICIT_CONSTRUCTORS(LAVAudioFilter)
	public:
		explicit LAVAudioFilter(FilterObserver* observer);
		virtual ~LAVAudioFilter();
		INT GetPinCount() OVERRIDE;
		IPin* GetPin(INT index) OVERRIDE;
	private:
		TinyScopedReferencePtr<LAVAudioInputPin> m_inputPin;
	};
}



