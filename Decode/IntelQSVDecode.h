#pragma once
#include "Common.h"
extern "C"
{
#include "libavutil/avutil.h"
#include "libavutil/hwcontext.h"
#include "libavutil/mem.h"
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
using namespace TinyUI;

namespace Decode
{
	/// <summary>
	/// Intel QSV½âÂë
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
		BOOL Close();
	private:
		AVCodec*		m_codec;
		AVPacket		m_packet;
		AVBufferRef*	m_bufferRef;
		AVCodecContext*	m_context;
	};
}



