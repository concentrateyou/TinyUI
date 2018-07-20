#include "stdafx.h"
#include "VideoCaptureParam.h"
#include "VideoCapture.h"

namespace DShow
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
		case PIXEL_FORMAT_MJPEG:
			return "MJPEG";
		case PIXEL_FORMAT_YV12:
			return "YV12";
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
	void VideoCaptureFormat::SetFormat(VideoPixelFormat vpf)
	{
		m_vpf = vpf;
	}
	void VideoCaptureFormat::SetSize(const TinySize& size)
	{
		m_size = size;
	}
	void VideoCaptureFormat::SetSize(INT cx, INT cy)
	{
		m_size.cx = cx;
		m_size.cy = cy;
	}
	void VideoCaptureFormat::SetScale(INT cx, INT cy)
	{
		m_scale.cx = cx;
		m_scale.cy = cy;
	}
	void VideoCaptureFormat::SetRate(FLOAT rate)
	{
		m_rate = rate;
	}
	const TinySize& VideoCaptureFormat::GetSize() const
	{
		return m_size;
	}
	const TinySize&	VideoCaptureFormat::GetScale() const
	{
		return m_scale;
	}
	const FLOAT VideoCaptureFormat::GetRate() const
	{
		return m_rate;
	}
	const VideoPixelFormat VideoCaptureFormat::GetFormat() const
	{
		return m_vpf;
	}
	const string VideoCaptureFormat::ToString() const
	{
		return StringPrintf("%s,%d*%d,%.3f", PixelFormatToString(m_vpf).c_str(), m_size.cx, m_size.cy, m_rate);
	}
	//////////////////////////////////////////////////////////////////////////
	VideoCaptureParam::VideoCaptureParam()
	{

	}
	VideoCaptureParam::~VideoCaptureParam()
	{

	}
}
