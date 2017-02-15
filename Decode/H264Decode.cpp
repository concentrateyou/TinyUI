#include "stdafx.h"
#include "H264Decode.h"

namespace Decode
{
	H264Decode::H264Decode()
		:m_pYUV420(NULL),
		m_context(NULL),
		m_codec(NULL),
		m_sws(NULL),
		m_pBGR24(NULL)
	{
		ZeroMemory(&m_packet, sizeof(m_packet));
	}

	H264Decode::~H264Decode()
	{
		Close();
	}
	BOOL H264Decode::Initialize(const TinySize& src, const TinySize& dst, Callback<void(BYTE*, LONG, LPVOID)>&& callback)
	{
		m_callback = std::move(callback);
		avcodec_register_all();
		av_init_packet(&m_packet);
		m_codec = avcodec_find_decoder(AV_CODEC_ID_H264);
		if (!m_codec)
			return FALSE;
		m_context = avcodec_alloc_context3(m_codec);
		if (!m_context)
			return FALSE;
		m_pYUV420 = av_frame_alloc();
		if (!m_pYUV420)
			goto H264_ERROR;
		m_pBGR24 = av_frame_alloc();
		if (!m_pBGR24)
			goto H264_ERROR;
		m_src = src;
		m_dst = dst;
		m_sws = sws_getContext(src.cx, src.cy, AV_PIX_FMT_YUV420P, dst.cx, dst.cy, AV_PIX_FMT_BGR24, 0, NULL, NULL, NULL);
		if (!m_sws)
			goto H264_ERROR;
		INT buffersize = av_image_get_buffer_size(AV_PIX_FMT_BGR24, m_dst.cx, m_dst.cy, 1);
		m_bits.Reset(new BYTE[buffersize]);
		ZeroMemory(m_bits, buffersize);
		return buffersize == av_image_fill_arrays(m_pBGR24->data, m_pBGR24->linesize, m_bits.Ptr(), AV_PIX_FMT_BGR24, m_dst.cx, m_dst.cy, 1);
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
		if (avcodec_send_packet(m_context, &m_packet) != 0)
			return FALSE;
		if (avcodec_receive_frame(m_context, m_pYUV420) != 0)
			return FALSE;
		sws_scale(m_sws, m_pYUV420->data, m_pYUV420->linesize, 0, m_src.cy, m_pBGR24->data, m_pBGR24->linesize);
		if (!m_callback.IsNull())
		{
			m_callback(m_pBGR24->data[0], m_pBGR24->linesize[0] * m_dst.cy, this);
		}
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
		if (m_pYUV420)
		{
			av_frame_free(&m_pYUV420);
			m_pYUV420 = NULL;
		}
		if (m_pBGR24)
		{
			av_frame_free(&m_pBGR24);
			m_pBGR24 = NULL;
		}
		return TRUE;
	}
}
