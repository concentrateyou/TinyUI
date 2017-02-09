#include "stdafx.h"
#include "H264Decode.h"

namespace Decode
{
	H264Decode::H264Decode()
		:m_av(NULL),
		m_rgb24(NULL),
		m_context(NULL),
		m_sws(NULL)
	{
		ZeroMemory(&m_packet, sizeof(m_packet));
	}

	H264Decode::~H264Decode()
	{
	}
	BOOL H264Decode::Open()
	{
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
		m_av = av_frame_alloc();
		if (!m_av)
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
				if (avcodec_receive_frame(m_context, m_av) != 0)
					return FALSE;
			}
		}
		return TRUE;
	}
	BOOL H264Decode::Close()
	{
		if (m_context)
		{
			avcodec_close(m_context);
			m_context = NULL;
		}
		return TRUE;
	}
	void H264Decode::OnDone(BYTE* bits, LONG size, LPVOID ps)
	{
		EVENT_DONE(bits, size, ps);
	}
}
