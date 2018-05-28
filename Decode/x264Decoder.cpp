#include "stdafx.h"
#include "x264Decoder.h"
#include "Common/TinyLogging.h"

namespace Decode
{
	x264Decoder::x264Decoder()
		:m_pYUV420(NULL),
		m_context(NULL),
		m_codec(NULL),
		m_sws(NULL),
		m_pRGB32(NULL)
	{
		ZeroMemory(&m_packet, sizeof(m_packet));
	}

	x264Decoder::~x264Decoder()
	{
		Close();
	}
	void log_callback(void*, int level, const char* format, va_list va)
	{
		if (level <= AV_LOG_ERROR)
		{
			char buf[1024] = { 0 };
			int r = ::vsnprintf(buf, 1024, format, va);
			TRACE("Error:%s\n", buf);
		}
	}
	BOOL x264Decoder::Initialize(const TinySize& srcsize, const TinySize& dstsize)
	{
		av_log_set_level(AV_LOG_ERROR);
		av_log_set_callback(log_callback);
		av_init_packet(&m_packet);
		m_srcsize = srcsize;
		m_dstsize = dstsize;
		m_pYUV420 = av_frame_alloc();
		if (!m_pYUV420)
			goto _ERROR;
		m_pRGB32 = av_frame_alloc();
		if (!m_pRGB32)
			goto _ERROR;
		m_sws = sws_getContext(srcsize.cx, srcsize.cy, AV_PIX_FMT_YUV420P, dstsize.cx, dstsize.cy, AV_PIX_FMT_RGB32, 0, NULL, NULL, NULL);
		if (!m_sws)
			goto _ERROR;
		INT size = av_image_get_buffer_size(AV_PIX_FMT_RGB32, m_dstsize.cx, m_dstsize.cy, 1);
		m_bits.Reset(new BYTE[size]);
		if (!m_bits)
			goto _ERROR;
		memset(m_bits, 0, size);
		return size == av_image_fill_arrays(m_pRGB32->data, m_pRGB32->linesize, m_bits.Ptr(), AV_PIX_FMT_RGB32, m_dstsize.cx, m_dstsize.cy, 1);
	_ERROR:
		Close();
		return FALSE;
	}
	BOOL x264Decoder::Open(BYTE* metadata, LONG size)
	{
		m_codec = avcodec_find_decoder(AV_CODEC_ID_H264);
		if (!m_codec)
			goto H264_ERROR;
		m_context = avcodec_alloc_context3(m_codec);
		if (!m_context)
			goto H264_ERROR;
		m_context->flags2 |= AV_CODEC_FLAG2_CHUNKS | AV_CODEC_FLAG2_FAST;
		if (m_codec->capabilities & AV_CODEC_CAP_TRUNCATED)
		{
			m_context->flags |= AV_CODEC_FLAG_TRUNCATED;
		}
		m_context->err_recognition = 0;
		m_context->workaround_bugs = FF_BUG_AUTODETECT;
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
	INT x264Decoder::Decode(SampleTag& tag, BYTE*& bo, LONG& so)
	{
		if (!m_context || !m_sws)
			return AVERROR(EINVAL);
		so = 0;
		bo = NULL;
		m_packet.dts = tag.sampleDTS;
		m_packet.pts = tag.samplePTS;
		m_packet.data = tag.bits;
		m_packet.size = tag.size;
		INT iRes = avcodec_send_packet(m_context, &m_packet);
		if (iRes != 0)
			goto _ERROR;
		while (iRes >= 0)
		{
			iRes = avcodec_receive_frame(m_context, m_pYUV420);
			if (iRes == AVERROR(EAGAIN) || iRes == AVERROR_EOF)
			{
				if (iRes == AVERROR_EOF)
				{
					avcodec_flush_buffers(m_context);
				}
				TRACE("x264 Decode MORE\n");
				LOG(INFO) << "x264 Decode MORE";
				break;
			}
			else if (iRes < 0)
			{
				TRACE("x264 Decode FAIL\n");
				LOG(ERROR) << "x264 Decode FAIL";
				goto _ERROR;
			}
			INT cy = sws_scale(m_sws, m_pYUV420->data, m_pYUV420->linesize, 0, m_srcsize.cy, m_pRGB32->data, m_pRGB32->linesize);
			ASSERT(cy == m_dstsize.cy);
			bo = m_pRGB32->data[0];
			so = m_pRGB32->linesize[0] * cy;
			tag.linesize = m_pRGB32->linesize[0];
			tag.cy = cy;
			goto _ERROR;
		}
	_ERROR:
		SAFE_DELETE_ARRAY(tag.bits);
		av_packet_unref(&m_packet);
		return iRes;
	}
	BOOL x264Decoder::Close()
	{
		m_srcsize.Empty();
		m_dstsize.Empty();
		if (m_context != NULL)
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
		if (m_sws != NULL)
		{
			sws_freeContext(m_sws);
			m_sws = NULL;
		}
		if (m_pYUV420 != NULL)
		{
			av_frame_free(&m_pYUV420);
			m_pYUV420 = NULL;
		}
		if (m_pRGB32 != NULL)
		{
			av_frame_free(&m_pRGB32);
			m_pRGB32 = NULL;
		}
		m_bits.Reset(NULL);
		return TRUE;
	}
	BOOL x264Decoder::Reset()
	{
		if (m_context != NULL)
		{
			avcodec_flush_buffers(m_context);
			return TRUE;
		}
		return FALSE;
	}
	AVFrame* x264Decoder::GetYUV420() const
	{
		return m_pYUV420;
	}
	AVFrame* x264Decoder::GetRGB32() const
	{
		return m_pRGB32;
	}
}
