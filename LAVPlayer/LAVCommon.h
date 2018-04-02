#pragma once
#include "DShowCommon.h"
#include "ScopedMediaType.h"
#include "FilterBase.h"
#include "FilterObserver.h"
#include <dshow.h>
#include <dvdmedia.h>
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
	BOOL WINAPI GetFilterByCLSID(const string& clsid, IBaseFilter** ps);
}