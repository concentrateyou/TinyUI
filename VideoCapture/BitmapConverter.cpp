#include "stdafx.h"
#include "BitmapConverter.h"

namespace Media
{
	BitmapConverter::BitmapConverter()
		:m_av(NULL),
		m_rgb24AV(NULL),
		m_sws(NULL),
		m_subtype(GUID_NULL),
		m_cx(0),
		m_cy(0),
		m_scaleCX(0),
		m_scaleCY(0)
	{
	}

	BOOL BitmapConverter::Initialize(GUID subtype, INT cx, INT cy, INT scaleCX, INT scaleCY)
	{
		m_subtype = subtype;
		m_cx = cx;
		m_cy = cy;
		m_scaleCX = scaleCX;
		m_scaleCY = scaleCY;
		m_av = av_frame_alloc();
		if (!m_av)
			return FALSE;
		m_rgb24AV = av_frame_alloc();
		if (!m_rgb24AV)
			return FALSE;
		m_sws = sws_getContext(cx, cy, GetPixelFormat(), scaleCX, scaleCY, AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
		if (!m_sws)
			return FALSE;
		return TRUE;
	}

	BitmapConverter::~BitmapConverter()
	{
		Release();
	}
	void BitmapConverter::Release()
	{
		if (m_av != NULL)
		{
			av_frame_free(&m_av);
			m_av = NULL;
		}
		if (m_rgb24AV != NULL)
		{
			av_frame_free(&m_rgb24AV);
			m_rgb24AV = NULL;
		}
		if (m_sws != NULL)
		{
			sws_freeContext(m_sws);
			m_sws = NULL;
		}
	}
	BOOL BitmapConverter::ToRGB24(const BYTE* ps, INT size, RGB24& rgb)
	{
		m_av->width = m_cx;
		m_av->height = m_cy;
		m_av->format = GetPixelFormat();
		int s = av_image_get_buffer_size((AVPixelFormat)m_av->format, m_av->width, m_av->height, 1);
		av_image_fill_linesizes(m_av->linesize, (AVPixelFormat)m_av->format, m_av->width);
		av_image_fill_arrays(m_av->data, m_av->linesize, ps, (AVPixelFormat)m_av->format, m_av->width, m_av->height, 1);
		m_rgb24AV->width = rgb.cx;
		m_rgb24AV->height = rgb.cy;
		m_rgb24AV->format = AV_PIX_FMT_RGB24;
		av_image_fill_linesizes(m_rgb24AV->linesize, (AVPixelFormat)m_rgb24AV->format, m_rgb24AV->width);
		av_image_fill_arrays(m_rgb24AV->data, m_rgb24AV->linesize, rgb.data, (AVPixelFormat)m_rgb24AV->format, m_rgb24AV->width, m_rgb24AV->height, 1);
		int size3 = sws_scale(m_sws, m_av->data, m_av->linesize, 0, m_av->height, m_rgb24AV->data, m_rgb24AV->linesize);
		return TRUE;
	}
	AVPixelFormat BitmapConverter::GetPixelFormat()
	{
		if (m_subtype == MEDIASUBTYPE_MJPG)
		{
			return AV_PIX_FMT_YUVJ420P;
		}
		if (m_subtype == MEDIASUBTYPE_YUY2)
		{
			return AV_PIX_FMT_YUYV422;
		}
		static const GUID MY_MEDIASUBTYPE_I420 = { 0x30323449, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };
		if (m_subtype == MY_MEDIASUBTYPE_I420)
		{
			return AV_PIX_FMT_YUV420P;
		}
		return AV_PIX_FMT_RGB24;
	}
}
