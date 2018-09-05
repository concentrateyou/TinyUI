#pragma once
#include "DShowCommon.h"
#include "LAVCommon.h"
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
	class LAVVideo;
	class LAVVideoInputPin : public InputPinBase
	{
		DISALLOW_IMPLICIT_CONSTRUCTORS(LAVVideoInputPin)
	public:
		LAVVideoInputPin(FilterBase* pFilter, FilterObserver* observer);
		virtual ~LAVVideoInputPin();
		const LAVVideoFormat& GetResponseFormat();
		void	SetRequestFormat(const LAVVideoFormat& request);
		HRESULT CheckMediaType(const AM_MEDIA_TYPE* pMediaType) OVERRIDE;
		HRESULT GetMediaType(INT position, AM_MEDIA_TYPE* pMediaType) OVERRIDE;
	private:
		LAVVideoFormat	m_request;
		LAVVideoFormat	m_response;
	};
}

