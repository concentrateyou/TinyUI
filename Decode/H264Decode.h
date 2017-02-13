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
		BOOL Open(const TinySize& src, const TinySize& dst, BYTE* avc1, LONG size);
		BOOL Decode(BYTE* bits, LONG size);
		BOOL Close();
	public:
		virtual void OnDone(BYTE*, LONG, LPVOID);
		Event<void(BYTE*, LONG, LPVOID)> EVENT_DONE;
	private:
		INT				m_outsize;
		TinySize		m_srcsize;
		TinySize		m_dstsize;
		SwsContext*		m_sws;
		AVPacket		m_packet;
		AVFrame*		m_i420;
		AVFrame*		m_rgb24;
		AVCodecContext* m_context;
		TinyScopedArray<BYTE>	m_bits;
	};
}



