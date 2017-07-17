#pragma once
#include "Common.h"
extern "C"
{
#include "libavutil/avutil.h"
#include "libavutil/hwcontext.h"
#include "libavutil/hwcontext_qsv.h"
#include "libavutil/mem.h"
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
using namespace TinyUI;

namespace Decode
{
	/// <summary>
	/// Intel QSV����
	/// </summary>
	class IntelQSVDecode
	{
		DISALLOW_COPY_AND_ASSIGN(IntelQSVDecode)
	public:
		IntelQSVDecode();
		virtual ~IntelQSVDecode();
	public:
		BOOL Initialize(const TinySize& srcsize, const TinySize& dstsize);
		BOOL Open(BYTE* metadata, LONG size);
		BOOL Decode(SampleTag& tag, BYTE*& bo, LONG& so);
		BOOL Close();
		AVFrame* GetYUV420() const;
		AVFrame* GetRGB32() const;
	private:
		TinySize				m_srcsize;
		TinySize				m_dstsize;
		AVPacket				m_packet;
		AVBufferRef*			m_bufferRef;
		AVCodec*				m_codec;
		AVFrame*				m_pYUV420;
		AVFrame*				m_pRGB32;
		AVCodecContext*			m_context;
		TinyScopedArray<BYTE>	m_bits;
	};
}



