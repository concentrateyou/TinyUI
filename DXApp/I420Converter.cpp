#include "stdafx.h"
#include "I420Converter.h"

I420Converter::I420Converter(INT cx, INT cy)
	:m_sws(NULL),
	m_i420(NULL),
	m_bgra(NULL),
	m_cx(cx),
	m_cy(cy),
	m_size(0)
{
	m_i420 = av_frame_alloc();
	m_bgra = av_frame_alloc();
	m_sws = sws_getContext(cx, cy, AV_PIX_FMT_BGRA, cx, cy, AV_PIX_FMT_YUV420P, 0, NULL, NULL, NULL);
}

I420Converter::~I420Converter()
{
	if (m_i420)
	{
		av_frame_free(&m_i420);
		m_i420 = NULL;
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
	ASSERT(m_sws && m_i420 && m_bgra);
	av_image_fill_arrays(m_bgra->data, m_bgra->linesize, pBGRA, AV_PIX_FMT_BGRA, m_cx, m_cy, 1);
	m_size = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, m_cx, m_cy, 1);
	if (m_bits.IsEmpty())
	{
		m_bits.Reset(new BYTE[m_size]);
	}
	ZeroMemory(m_bits, m_size*sizeof(BYTE));
	av_image_fill_arrays(m_i420->data, m_i420->linesize, m_bits.Ptr(), AV_PIX_FMT_YUV420P, m_cx, m_cy, 1);
	sws_scale(m_sws, m_bgra->data, m_bgra->linesize, 0, m_cy, m_i420->data, m_i420->linesize);

	/*FILE* fileHandle = NULL;
	INT cx = m_cx;
	INT cy = m_cy;
	fopen_s(&fileHandle, "D:\\1234.yuv", "wb");
	for (INT y = 0; y < cy; y++)
	{
		fwrite(m_i420->data[0] + y*m_i420->linesize[0], 1, m_cx, fileHandle);
	}
	cx >>= 1;
	cy >>= 1;
	for (INT y = 0; y < cy; y++)
	{
		fwrite(m_i420->data[1] + y*m_i420->linesize[1], 1, cx, fileHandle);
	}
	for (INT y = 0; y < cy; y++)
	{
		fwrite(m_i420->data[2] + y*m_i420->linesize[2], 1, cx, fileHandle);
	}
	fclose(fileHandle);*/

	return TRUE;
}

AVFrame* I420Converter::GetI420() const
{
	return m_i420;
}
