#pragma once
#include "Utility.h"
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
using namespace TinyUI;

namespace Decode
{
	class H264Decode
	{
	public:
		H264Decode();
		virtual ~H264Decode();
		BOOL Open();
		BOOL Decode(BYTE* bits, LONG size);
		BOOL Close();
	public:
		virtual void OnDone(BYTE*, LONG, LPVOID);
		Event<void(BYTE*, LONG, LPVOID)> EVENT_DONE;
	private:
		AVPacket		m_packet;
		AVFrame*		m_av;
		AVFrame*		m_rgb24;
		AVCodecContext* m_context;
		SwsContext*		m_sws;
	};
}



