#pragma once
#include "DShowCommon.h"
#include "ScopedMediaType.h"
#include "FilterBase.h"
#include "FilterObserver.h"
#include "lav/ITrackInfo.h"
#include <dshow.h>
#include <dvdmedia.h>
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
	BOOL WINAPI GetFilterByCLSID(const string& clsid, IBaseFilter** ps);

	class LAVAudioParam
	{
		DISALLOW_COPY_AND_ASSIGN(LAVAudioParam)
	};

	class LAVVideoParam
	{
		DISALLOW_COPY_AND_ASSIGN(LAVVideoParam)
	};

	class ScopedBSTR
	{
		DISALLOW_COPY_AND_ASSIGN(ScopedBSTR)
	public:
		ScopedBSTR(const CHAR* psText);
		~ScopedBSTR();
	private:
		BSTR	m_bstr;
	};
}