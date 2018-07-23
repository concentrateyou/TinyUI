#pragma once
#include "Common/TinyEvent.h"
#include "Common/TinyString.h"
#include "Common/TinyCallback.h"
#include "Common/TinyCore.h"
#include "Render/TinyGDI.h"
#include "Media/TinyMedia.h"
#include <mmsystem.h>
#include <dshow.h>
#include <MMDeviceAPI.h>
#include <AudioClient.h>
#include <AudioPolicy.h>
#include <avrt.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mferror.h>
#include <mfcaptureengine.h>
#include <Mftransform.h>
#include <Wmcodecdsp.h>
#include <mfplay.h>
#include <mfreadwrite.h>
#include <mmdeviceapi.h>
#include <Wmcodecdsp.h>
#include <functiondiscoverykeys.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "avrt.lib")
#pragma comment(lib, "Mfplat.lib")
#pragma comment(lib, "Mfuuid.lib")
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "avrt.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfplay.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "wmcodecdspuuid.lib")

using namespace TinyFramework;
using namespace TinyFramework::Media;

namespace MF
{
	BOOL IsYuvPlanar(VideoPixelFormat format);
	BOOL IsOpaque(VideoPixelFormat format);

	HRESULT CopyAttribute(IMFAttributes *pSrc, IMFAttributes *pDest, const GUID& key);
	HRESULT CloneVideoMediaType(IMFMediaType *pSrcMediaType, REFGUID guidSubType, IMFMediaType **ppNewMediaType);

}