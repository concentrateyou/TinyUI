#include "stdafx.h"
#include "VideoCaptureParam.h"
#include "VideoCapture.h"

namespace Media
{
	VideoCaptureParam::VideoCaptureParam()
		:m_rate(0.0F), m_vpf(PIXEL_FORMAT_UNKNOWN)
	{

	}
	VideoCaptureParam::VideoCaptureParam(const TinySize& size, FLOAT rate, VideoPixelFormat vpf)
		: m_size(size),
		m_rate(rate),
		m_vpf(vpf)
	{

	}
	string VideoCaptureParam::PixelFormatToString(VideoPixelFormat vpf)
	{
		switch (vpf)
		{
		case PIXEL_FORMAT_UNKNOWN:
			return "UNKNOWN";
		case PIXEL_FORMAT_I420:
			return "I420";
		case PIXEL_FORMAT_YUY2:
			return "YUY2";
		case PIXEL_FORMAT_UYVY:
			return "UYUY";
		case PIXEL_FORMAT_RGB24:
			return "RGB24";
		case PIXEL_FORMAT_ARGB:
			return "ARGB";
		case PIXEL_FORMAT_MJPEG:
			return "MJPEG";
		case PIXEL_FORMAT_NV21:
			return "YV12";
		case PIXEL_FORMAT_YV12:
			return "YV12";
		case PIXEL_FORMAT_TEXTURE:
			return "TEXTURE";
		case PIXEL_FORMAT_MAX:
			break;
		}
		return "";
	}
	BOOL VideoCaptureParam::IsValid() const
	{
		return (m_size.cx < MaxDimension) &&
			(m_size.cy < MaxDimension) &&
			(m_size.cx * m_size.cy >= 0) &&
			(m_size.cx * m_size.cy < MaxCanvas) &&
			(m_rate >= 0.0F) &&
			(m_rate < MaxFramesPerSecond) &&
			(m_vpf >= PIXEL_FORMAT_UNKNOWN) &&
			(m_vpf < PIXEL_FORMAT_MAX);
	}
	void VideoCaptureParam::SetFormat(VideoPixelFormat vpf)
	{
		m_vpf = vpf;
	}
	void VideoCaptureParam::SetSize(const TinySize& size)
	{
		m_size = size;
	}
	void VideoCaptureParam::SetSize(INT cx, INT cy)
	{
		m_size.cx = cx;
		m_size.cy = cy;
	}
	void VideoCaptureParam::SetRate(FLOAT rate)
	{
		m_rate = rate;
	}
	const TinySize& VideoCaptureParam::GetSize() const
	{
		return m_size;
	}
	FLOAT VideoCaptureParam::GetRate() const
	{
		return m_rate;
	}
	VideoPixelFormat VideoCaptureParam::GetFormat() const
	{
		return m_vpf;
	}
	string VideoCaptureParam::ToString() const
	{
		return StringPrintf("%s,%d*%d", PixelFormatToString(m_vpf).c_str(), m_size.cx, m_size.cy);
	}
}
