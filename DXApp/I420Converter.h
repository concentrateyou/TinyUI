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
#define DEBUG_FFMPEG 1
#if DEBUG_FFMPEG
#pragma comment(lib, "libavcodecd.lib")
#pragma comment(lib, "libavdeviced.lib")
#pragma comment(lib, "libpostprocd.lib")
#pragma comment(lib, "libavfilterd.lib")
#pragma comment(lib, "libavformatd.lib")
#pragma comment(lib, "libavutild.lib")
#pragma comment(lib, "libswscaled.lib")
#else
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swscale.lib")
#endif

namespace DXApp
{
	class I420Converter
	{
		DISALLOW_COPY_AND_ASSIGN(I420Converter)
	public:
		I420Converter(const TinySize& srcSize, const TinySize& dstSize);
		~I420Converter();
		BOOL		BRGAToI420(BYTE* pBGRA);
		AVFrame*	GetI420() const;
	private:
		AVFrame*				m_i420;
		AVFrame*				m_bgra;
		SwsContext*				m_sws;
		TinySize				m_srcSize;
		TinySize				m_dstSize;
		INT						m_size;
		TinyScopedArray<BYTE>	m_bits;
	};
}
