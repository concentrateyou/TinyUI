#pragma once
#include "Utility.h"
extern "C"
{
#include "libavutil/avutil.h"
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
	class H264Decode
	{
	public:
		H264Decode();
		virtual ~H264Decode();
		BOOL Initialize(const TinySize& src, const TinySize& dst, Callback<void(BYTE*, LONG, LPVOID)>&& callback);
		BOOL Open(BYTE* metadata, LONG size);
		BOOL Decode(BYTE* data, LONG size, LONG dts, LONG pts);
		BOOL Close();
	private:
		TinySize		m_src;
		TinySize		m_dst;
		AVPacket		m_packet;
		AVFrame*		m_pYUV420;
		AVFrame*		m_pBGR24;
		AVCodec*		m_codec;
		SwsContext*		m_sws;
		AVCodecContext* m_context;
		TinyScopedArray<BYTE>	m_bits;
		Callback<void(BYTE*, LONG, LPVOID)> m_callback;
	};
}



