#pragma once
#include "VideoCommon.h"
#define __STDC_CONSTANT_MACROS  
extern "C"
{
#include "libavutil/imgutils.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
};
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swscale.lib")

namespace Media
{
	class BitmapConverter
	{
		DISALLOW_COPY_AND_ASSIGN(BitmapConverter);
	public:
		BitmapConverter(GUID subtype, INT cx, INT cy, INT scaleCX, INT scaleCY);
		~BitmapConverter();
		BOOL ToRGB24(const BYTE* sample, INT size, BYTE* pvBits);
	private:
		INT				m_cx;
		INT				m_cy;
		INT				m_scaleCX;
		INT				m_scaleCY;
		AVFrame*		m_srcAV;
		AVFrame*		m_dstAV;
		SwsContext*		m_sws;
		AVPixelFormat	m_pixelFormat;
	};
}


