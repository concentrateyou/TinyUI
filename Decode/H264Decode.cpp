#include "stdafx.h"
#include "H264Decode.h"

namespace Decode
{
	H264Decode::H264Decode()
		:m_yuv420(NULL),
		m_context(NULL),
		m_codec(NULL),
		m_sws(NULL),
		m_bgr24(NULL)
	{
		ZeroMemory(&m_packet, sizeof(m_packet));
	}

	H264Decode::~H264Decode()
	{
		Close();
	}
	BOOL H264Decode::Initialize(const TinySize& size, const TinySize& scale)
	{
		avcodec_register_all();
		av_init_packet(&m_packet);
		m_codec = avcodec_find_decoder(AV_CODEC_ID_H264);
		if (!m_codec)
			return FALSE;
		m_context = avcodec_alloc_context3(m_codec);
		if (!m_context)
			return FALSE;
		m_yuv420 = av_frame_alloc();
		if (!m_yuv420)
			goto H264_ERROR;
		m_bgr24 = av_frame_alloc();
		if (!m_bgr24)
			goto H264_ERROR;
		m_size = size;
		m_scale = scale;
		m_sws = sws_getContext(size.cx, size.cy, AV_PIX_FMT_YUV420P, scale.cx, scale.cy, AV_PIX_FMT_BGR24, 0, NULL, NULL, NULL);
		if (!m_sws)
			goto H264_ERROR;
		INT buffersize = av_image_get_buffer_size(AV_PIX_FMT_BGR24, m_scale.cx, m_scale.cy, 1);
		m_bits.Reset(new BYTE[buffersize]);
		ZeroMemory(m_bits, buffersize);
		ASSERT(buffersize == av_image_fill_arrays(m_bgr24->data, m_bgr24->linesize, m_bits.Ptr(), AV_PIX_FMT_BGR24, m_scale.cx, m_scale.cy, 1));
		return TRUE;
	H264_ERROR:
		Close();
		return FALSE;

	}
	BOOL H264Decode::Open(BYTE* metadata, LONG size)
	{
		ASSERT(m_context);
		m_context->extradata_size = size;
		m_context->extradata = reinterpret_cast<BYTE*>(av_malloc(m_context->extradata_size));
		memcpy(m_context->extradata, metadata, m_context->extradata_size);
		if (avcodec_open2(m_context, m_codec, NULL) != 0)
			goto H264_ERROR;
		return TRUE;
	H264_ERROR:
		Close();
		return FALSE;
	}
	BOOL H264Decode::Decode(BYTE* bits, LONG size)
	{
		if (!m_context)
			return FALSE;
		m_packet.data = bits;
		m_packet.size = size;
		INT iRes = avcodec_send_packet(m_context, &m_packet);
		if (iRes != 0)
			return FALSE;
		iRes = avcodec_receive_frame(m_context, m_yuv420);
		if (iRes != 0)
			return FALSE;
		sws_scale(m_sws, m_yuv420->data, m_yuv420->linesize, 0, m_size.cy, m_bgr24->data, m_bgr24->linesize);
		BITMAPINFOHEADER bih;
		bih.biSize = sizeof(BITMAPINFOHEADER);
		bih.biWidth = m_scale.cx;
		bih.biHeight = -m_scale.cy;
		bih.biPlanes = 1;
		bih.biBitCount = 24;
		bih.biCompression = BI_RGB;
		bih.biSizeImage = 0;
		bih.biXPelsPerMeter = 0;
		bih.biYPelsPerMeter = 0;
		bih.biClrUsed = 0;
		bih.biClrImportant = 0;
		Utility::SaveBitmap(bih, m_bits, m_bgr24->linesize[0] * m_scale.cy);
		return TRUE;
	}
	BOOL H264Decode::Close()
	{
		if (m_context)
		{
			if (m_context->extradata)
			{
				av_free(m_context->extradata);
				m_context->extradata = NULL;
			}
			avcodec_close(m_context);
			avcodec_free_context(&m_context);
			m_context = NULL;
			m_codec = NULL;
		}
		if (m_sws)
		{
			sws_freeContext(m_sws);
			m_sws = NULL;
		}
		if (m_yuv420)
		{
			av_frame_free(&m_yuv420);
			m_yuv420 = NULL;
		}
		if (m_bgr24)
		{
			av_frame_free(&m_bgr24);
			m_bgr24 = NULL;
		}
		return TRUE;
	}
	void H264Decode::OnDone(BYTE* bits, LONG size, LPVOID ps)
	{
		EVENT_DONE(bits, size, ps);
	}
}
