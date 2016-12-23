#pragma once
#include "Common/TinyEvent.h"
#include "Common/TinyString.h"
#include "Common/TinyCallback.h"
#include "Common/TinyCore.h"
#include "Render/TinyGDI.h"
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

using namespace TinyUI;

namespace MF
{
	enum VideoPixelFormat
	{
		PIXEL_FORMAT_UNKNOWN = 0,
		PIXEL_FORMAT_I420 = 1,
		PIXEL_FORMAT_YV12 = 2,
		PIXEL_FORMAT_YV16 = 3,
		PIXEL_FORMAT_YV12A = 4,
		PIXEL_FORMAT_YV24 = 5,
		PIXEL_FORMAT_NV12 = 6,
		PIXEL_FORMAT_NV21 = 7,
		PIXEL_FORMAT_UYVY = 8,
		PIXEL_FORMAT_YUY2 = 9,
		PIXEL_FORMAT_ARGB = 10,
		PIXEL_FORMAT_XRGB = 11,
		PIXEL_FORMAT_RGB24 = 12,
		PIXEL_FORMAT_RGB32 = 13,
		PIXEL_FORMAT_MJPEG = 14,
		PIXEL_FORMAT_MT21 = 15,
		PIXEL_FORMAT_YUV420P9 = 16,
		PIXEL_FORMAT_YUV420P10 = 17,
		PIXEL_FORMAT_YUV422P9 = 18,
		PIXEL_FORMAT_YUV422P10 = 19,
		PIXEL_FORMAT_YUV444P9 = 20,
		PIXEL_FORMAT_YUV444P10 = 21,
		PIXEL_FORMAT_MAX = PIXEL_FORMAT_YUV444P10,
	};

	enum ColorSpace
	{
		COLOR_SPACE_UNSPECIFIED = 0,
		COLOR_SPACE_JPEG = 1,
		COLOR_SPACE_HD_REC709 = 2,
		COLOR_SPACE_SD_REC601 = 3,
		COLOR_SPACE_MAX = COLOR_SPACE_SD_REC601,
	};
	BOOL IsYuvPlanar(VideoPixelFormat format);
	BOOL IsOpaque(VideoPixelFormat format);
}