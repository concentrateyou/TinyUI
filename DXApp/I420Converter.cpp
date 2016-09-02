#include "stdafx.h"
#include "I420Converter.h"

I420Converter::I420Converter(INT cx, INT cy)
	:m_sws(NULL),
	m_yuv(NULL),
	m_bgra(NULL),
	m_cx(cx),
	m_cy(cy),
	m_size(0)
{
	m_yuv = av_frame_alloc();
	m_bgra = av_frame_alloc();
	m_sws = sws_getContext(cx, cy, AV_PIX_FMT_BGRA, cx, cy, AV_PIX_FMT_YUV420P, 0, NULL, NULL, NULL);
}

I420Converter::~I420Converter()
{
	if (m_yuv)
	{
		av_frame_free(&m_yuv);
		m_yuv = NULL;
	}
	if (m_bgra)
	{
		av_frame_free(&m_bgra);
		m_bgra = NULL;
	}
	if (m_sws)
	{
		sws_freeContext(m_sws);
		m_sws = NULL;
	}
}

BOOL I420Converter::BRGAToI420(BYTE* pBGRA)
{
	ASSERT(m_sws && m_yuv && m_bgra);
	av_image_fill_arrays(m_bgra->data, m_bgra->linesize, pBGRA, AV_PIX_FMT_BGRA, m_cx, m_cy, 1);
	m_size = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, m_cx, m_cy, 1);
	if (m_bits.IsEmpty())
	{
		m_bits.Reset(new BYTE[m_size]);
	}
	ZeroMemory(m_bits, m_size*sizeof(BYTE));
	av_image_fill_arrays(m_yuv->data, m_yuv->linesize, m_bits.Ptr(), AV_PIX_FMT_YUV420P, m_cx, m_cy, 1);
	sws_scale(m_sws, m_bgra->data, m_bgra->linesize, 0, m_cy, m_yuv->data, m_yuv->linesize);
	return TRUE;
}

AVFrame* I420Converter::GetI420() const
{
	return m_yuv;
}
