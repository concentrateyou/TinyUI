#include "stdafx.h"
#include "VideoCaptureFormat.h"
#include "VideoCaptureDevice.h"

namespace Media
{
	VideoCaptureFormat::VideoCaptureFormat()
		:m_rate(0.0F), m_vpf(PIXEL_FORMAT_UNKNOWN)
	{

	}
	VideoCaptureFormat::VideoCaptureFormat(const TinySize& size, FLOAT rate, VideoPixelFormat vpf)
		: m_size(size),
		m_rate(rate),
		m_vpf(vpf)
	{

	}
	string VideoCaptureFormat::PixelFormatToString(VideoPixelFormat vpf)
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
	BOOL VideoCaptureFormat::IsValid() const
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
}
