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
		BOOL Initialize(const TinySize& size, const TinySize& scale);
		BOOL Open(BYTE* metadata, LONG size);
		BOOL Decode(BYTE* bits, LONG size);
		BOOL Close();
	public:
		virtual void OnDone(BYTE*, LONG, LPVOID);
		Event<void(BYTE*, LONG, LPVOID)> EVENT_DONE;
	private:
		TinySize		m_size;
		TinySize		m_scale;
		AVPacket		m_packet;
		AVFrame*		m_yuv420;
		AVFrame*		m_bgr24;
		AVCodec*		m_codec;
		AVCodecContext* m_context;
		SwsContext*		m_sws;
		TinyScopedArray<BYTE>	m_bits;
	};
}



