#pragma once
#include "DXFramework.h"
extern "C"
{
#include "libavutil/avutil.h"
#include "libavutil/imgutils.h"
#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
}
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swscale.lib")

class I420Converter
{
	DISALLOW_COPY_AND_ASSIGN(I420Converter);
public:
	I420Converter(INT cx, INT cy);
	~I420Converter();
	BOOL		BRGAToI420(BYTE* pBGRA);
	AVFrame*	GetI420() const;
private:
	AVFrame*				m_yuv;
	AVFrame*				m_bgra;
	SwsContext*				m_sws;
	INT						m_cx;
	INT						m_cy;
	INT						m_size;
	TinyScopedArray<BYTE>	m_bits;
};

