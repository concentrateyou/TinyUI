#include "stdafx.h"
#include "BitmapConverter.h"

namespace Media
{
	static const GUID MEDIASUBTYPE_I420 =
	{ 0x30323449, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };

	BitmapConverter::BitmapConverter(GUID subtype, INT cx, INT cy, INT scaleCX, INT scaleCY)
		:m_cx(cx),
		m_cy(cy),
		m_scaleCX(scaleCX),
		m_scaleCY(scaleCY),
		m_srcAV(NULL),
		m_dstAV(NULL),
		m_sws(NULL),
		m_pixelFormat(AV_PIX_FMT_NONE)
	{
		m_srcAV = av_frame_alloc();
		m_dstAV = av_frame_alloc();
		if (subtype == MEDIASUBTYPE_YUY2)
			m_pixelFormat = AV_PIX_FMT_YUYV422;
		if (subtype == MEDIASUBTYPE_MJPG)
			m_pixelFormat = AV_PIX_FMT_YUVJ422P;
		if (subtype == MEDIASUBTYPE_I420)
			m_pixelFormat = AV_PIX_FMT_YUV420P;
		m_sws = sws_getContext(cx, cy, m_pixelFormat, scaleCX, scaleCY, AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
	}
	BitmapConverter::~BitmapConverter()
	{
		if (m_srcAV != NULL)
		{
			av_frame_free(&m_srcAV);
			m_srcAV = NULL;
		}
		if (m_dstAV != NULL)
		{
			av_frame_free(&m_dstAV);
			m_dstAV = NULL;
		}
		if (m_sws != NULL)
		{
			sws_freeContext(m_sws);
			m_sws = NULL;
		}
	}
	BOOL BitmapConverter::ToRGB24(const BYTE* sample, INT size, BYTE* pBits)
	{
		avpicture_fill(reinterpret_cast<AVPicture *>(m_srcAV), const_cast<uint8_t *>(sample), m_pixelFormat, m_cx, m_cy);
		avpicture_fill(reinterpret_cast<AVPicture *>(m_dstAV), pBits, AV_PIX_FMT_RGB24, m_scaleCX, m_scaleCY);
		m_dstAV->linesize[0] = m_scaleCX * 3;
		m_srcAV->data[0] += m_srcAV->linesize[0] * (m_cy - 1);
		m_srcAV->linesize[0] *= -1;
		if (m_sws != NULL)
		{
			INT i = sws_scale(m_sws, m_srcAV->data, m_srcAV->linesize, 0, m_cy, m_dstAV->data, m_dstAV->linesize);
		}
		return TRUE;
	}
}
