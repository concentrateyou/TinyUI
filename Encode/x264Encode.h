#pragma once
#include "Common/TinyCommon.h"
#include "Common/TinyCallback.h"
#include "Common/TinyEvent.h"
#include "Render/TinyGDI.h"
extern "C"
{
#include "x264_config.h"
#include "x264.h"
#include "libavutil/avutil.h"
#include "libavutil/imgutils.h"
#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
}
#pragma comment(lib,"libx264.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swscale.lib")

#define SPS_SEP 4
#define PPS_SEP 4
using namespace TinyUI;

namespace Encode
{
	typedef struct tagMediaTag
	{
		DWORD		dwType;
		DWORD		dwFlag;
		DWORD		dwTime;
		LONGLONG	INC;
		LONGLONG	PTS;
		LONGLONG	DTS;
	}MediaTag;

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

	class x264Encode
	{
		DISALLOW_COPY_AND_ASSIGN(x264Encode)
	public:
		x264Encode();
		virtual ~x264Encode();
	public:
		BOOL	Open(INT cx, INT cy, INT fps = 50, INT rate = 1000);//rateÆ½¾ùÂëÂÊ
		BOOL	Encode(AVFrame* pI420);
		void	Close();
	private:
		BOOL	BuildParam(INT cx, INT cy, INT fps, INT bitrate);
	private:
		x264_t*				m_x264;
		x264_param_t*		m_x264Param;
		x264_picture_t*		m_x264Image;
		LONGLONG			m_sINC;
		LONGLONG			m_sPTS;
	};
}
