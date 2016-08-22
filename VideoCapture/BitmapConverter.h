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
	struct RGB24
	{
		BYTE*	data;
		INT		cx;
		INT		cy;
	};
	class BitmapConverter
	{
	public:
		BitmapConverter();
		~BitmapConverter();
		BOOL Initialize(GUID subtype, INT cx, INT cy, INT scaleCX, INT scaleCY);
		BOOL ToRGB24(const BYTE* ps, INT size, RGB24& rgb);
		AVPixelFormat GetPixelFormat();
		void Release();
	private:
		INT					m_cx;
		INT					m_cy;
		INT					m_scaleCX;
		INT					m_scaleCY;
		GUID				m_subtype;
		AVFrame*			m_av;
		AVFrame*			m_rgb24AV;
		SwsContext*			m_sws;
	};
}


