#pragma once
#include "DShowCommon.h"
#include "ScopedMediaType.h"
#include "FilterObserver.h"
#include "LAVVideoInputPin.h"
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
	class __declspec(uuid("3C6FD87F-B4DF-4811-8C98-CE69163744E0"))
		LAVVideoFilter : public FilterBase
	{
		DISALLOW_IMPLICIT_CONSTRUCTORS(LAVVideoFilter)
	public:
		explicit LAVVideoFilter(FilterObserver* observer);
		virtual ~LAVVideoFilter();
		INT GetPinCount() OVERRIDE;
		IPin* GetPin(INT index) OVERRIDE;
	private:
		TinyScopedReferencePtr<LAVVideoInputPin> m_inputPin;
	};
}


