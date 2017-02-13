#include "stdafx.h"
#include "H264Decode.h"

namespace Decode
{
	H264Decode::H264Decode()
		:m_i420(NULL),
		m_rgb24(NULL),
		m_context(NULL),
		m_sws(NULL)
	{
		ZeroMemory(&m_packet, sizeof(m_packet));

	}

	H264Decode::~H264Decode()
	{
	}
	BOOL H264Decode::Open(const TinySize& src, const TinySize& dst)
	{
		m_srcSize = src;
		m_dstSize = dst;
		m_outsize = av_image_get_buffer_size(AV_PIX_FMT_RGB24, m_dstSize.cx, m_dstSize.cy, 1);
		m_bits.Reset(new BYTE[m_outsize]);
		m_sws = sws_getContext(src.cx, src.cy, AV_PIX_FMT_YUV420P, dst.cx, dst.cy, AV_PIX_FMT_RGB24, 0, NULL, NULL, NULL);
		if (!m_sws)
			return FALSE;
		avcodec_register_all();
		av_init_packet(&m_packet);
		AVCodec* codec = NULL;
		codec = avcodec_find_decoder(AV_CODEC_ID_H264);
		if (!codec)
			return FALSE;
		m_context = avcodec_alloc_context3(codec);
		if (!m_context)
			goto _ERROR;
		if (codec->capabilities & CODEC_CAP_TRUNCATED)
			m_context->flags |= CODEC_FLAG_TRUNCATED;
		if (avcodec_open2(m_context, codec, NULL) < 0)
			goto _ERROR;
		m_i420 = av_frame_alloc();
		if (!m_i420)
			goto _ERROR;
		return TRUE;
	_ERROR:
		avcodec_close(m_context);
		m_context = NULL;
		return FALSE;
	}
	BOOL H264Decode::Decode(BYTE* bits, LONG size)
	{
		if (!m_context)
			return FALSE;
		m_packet.size = size;
		m_packet.data = bits;
		for (;;)
		{
			if (m_packet.size > 0)
			{
				if (avcodec_send_packet(m_context, &m_packet) != 0)
					return FALSE;
				if (avcodec_receive_frame(m_context, m_i420) != 0)
					return FALSE;
				av_image_fill_arrays(m_rgb24->data, m_rgb24->linesize, m_bits.Ptr(), AV_PIX_FMT_RGB24, m_dstSize.cx, m_dstSize.cy, 1);
				sws_scale(m_sws, m_i420->data, m_i420->linesize, 0, m_srcSize.cy, m_rgb24->data, m_rgb24->linesize);
				BITMAPINFOHEADER bi;
				bi.biSize = sizeof(BITMAPINFOHEADER);
				bi.biWidth = m_dstSize.cx;
				bi.biHeight = m_dstSize.cy;
				bi.biPlanes = 1;
				bi.biBitCount = 24;
				bi.biCompression = BI_RGB;
				bi.biSizeImage = 0;
				bi.biXPelsPerMeter = 0;
				bi.biYPelsPerMeter = 0;
				bi.biClrUsed = 0;
				bi.biClrImportant = 0;
				Utility::SaveBitmap(bi, m_bits, m_outsize);
			}
		}
		return TRUE;
	}
	BOOL H264Decode::Close()
	{
		if (m_context)
		{
			avcodec_close(m_context);
			avcodec_free_context(&m_context);
			m_context = NULL;
		}
		if (m_i420)
		{
			av_frame_free(&m_i420);
			m_i420 = NULL;
		}
		return TRUE;
	}
	void H264Decode::OnDone(BYTE* bits, LONG size, LPVOID ps)
	{
		EVENT_DONE(bits, size, ps);
	}
}
