#include "stdafx.h"
#include "IntelQSVDecode.h"

namespace Decode
{
	static enum AVPixelFormat GetFormat(AVCodecContext *avctx, const enum AVPixelFormat *pFormat)
	{
		while (*pFormat != AV_PIX_FMT_NONE)
		{
			const void *hwcfg = NULL;
			AVHWFramesConstraints *constraints = av_hwdevice_get_hwframe_constraints((AVBufferRef*)avctx->opaque, hwcfg);
			const AVPixelFormat* inputs = constraints->valid_sw_formats;
			AVPixelFormat format = AV_PIX_FMT_NV12;
			if (inputs != NULL)
			{
				format = inputs[0];
			}
			av_hwframe_constraints_free(&constraints);
			if (*pFormat == AV_PIX_FMT_QSV)
			{
				AVBufferRef*bufferRef = (AVBufferRef*)avctx->opaque;
				INT iRes = 0;
				avctx->hw_frames_ctx = av_hwframe_ctx_alloc(bufferRef);
				if (!avctx->hw_frames_ctx)
					return AV_PIX_FMT_NONE;
				AVHWFramesContext* avhwctx = (AVHWFramesContext*)avctx->hw_frames_ctx->data;
				AVQSVFramesContext* avqsvcts = (AVQSVFramesContext*)avhwctx->hwctx;
				avhwctx->format = AV_PIX_FMT_QSV;
				avhwctx->sw_format = avctx->sw_pix_fmt;
				avhwctx->width = FFALIGN(avctx->coded_width, 32);
				avhwctx->height = FFALIGN(avctx->coded_height, 32);
				avhwctx->initial_pool_size = 32;
				avqsvcts->frame_type = MFX_MEMTYPE_VIDEO_MEMORY_DECODER_TARGET;
				iRes = av_hwframe_ctx_init(avctx->hw_frames_ctx);
				if (iRes < 0)
					return AV_PIX_FMT_NONE;
				return AV_PIX_FMT_QSV;
			}
			pFormat++;
		}
		return AV_PIX_FMT_NONE;
	}

	IntelQSVDecode::IntelQSVDecode()
		:m_codec(NULL),
		m_context(NULL),
		m_bufferRef(NULL),
		m_pYUV420(NULL),
		m_pRGB32(NULL)
	{
	}


	IntelQSVDecode::~IntelQSVDecode()
	{
	}

	BOOL IntelQSVDecode::Initialize(const TinySize& srcsize, const TinySize& dstsize)
	{
		avcodec_register_all();
		av_init_packet(&m_packet);
		m_srcsize = srcsize;
		m_dstsize = dstsize;
		m_pYUV420 = av_frame_alloc();
		if (!m_pYUV420)
			goto _ERROR;
		m_pRGB32 = av_frame_alloc();
		if (!m_pRGB32)
			goto _ERROR;
		return TRUE;
	_ERROR:
		Close();
		return FALSE;
	}

	BOOL IntelQSVDecode::Open(BYTE* metadata, LONG size)
	{
		if (av_hwdevice_ctx_create(&m_bufferRef, AV_HWDEVICE_TYPE_QSV, "auto", NULL, 0) < 0)
			goto _ERROR;
		m_codec = avcodec_find_decoder_by_name("h264_qsv");
		if (!m_codec)
			goto _ERROR;
		m_context = avcodec_alloc_context3(m_codec);
		if (!m_context)
			goto _ERROR;
		m_context->extradata_size = size;
		m_context->extradata = reinterpret_cast<BYTE*>(av_malloc(m_context->extradata_size + AV_INPUT_BUFFER_PADDING_SIZE));
		memcpy(m_context->extradata, metadata, m_context->extradata_size);
		m_context->opaque = m_bufferRef;
		m_context->get_format = GetFormat;
		m_context->refcounted_frames = 1;
		m_context->codec_id = AV_CODEC_ID_H264;
		m_context->pix_fmt = AV_PIX_FMT_QSV;
		m_context->codec_type = AVMEDIA_TYPE_VIDEO;
		INT iRes = avcodec_open2(m_context, m_codec, NULL);
		if (iRes < 0)
			goto _ERROR;
		return TRUE;
	_ERROR:
		Close();
		return FALSE;
	}

	BOOL IntelQSVDecode::Decode(SampleTag& tag, BYTE*& bo, LONG& so)
	{
		if (!m_context)
			return FALSE;
		INT iRes = 0;
		av_new_packet(&m_packet, tag.size);
		m_packet.dts = tag.sampleDTS;
		m_packet.pts = tag.samplePTS;
		memcpy(m_packet.data, tag.bits, tag.size);
		iRes = avcodec_send_packet(m_context, &m_packet);
		if (iRes != 0)
			goto _ERROR;
		while (iRes >= 0)
		{
			iRes = avcodec_receive_frame(m_context, m_pYUV420);
			if (iRes == AVERROR(EAGAIN) || iRes == AVERROR_EOF)
				break;
			else if (iRes < 0)
				goto _ERROR;
			iRes = av_hwframe_transfer_data(m_pRGB32, m_pYUV420, 0);
			if (iRes != 0)
			{
				av_frame_unref(m_pYUV420);
				av_frame_unref(m_pRGB32);
			}
			bo = m_pRGB32->data[0];
			so = m_pRGB32->linesize[0] * m_dstsize.cy;
			goto _ERROR;
		}
	_ERROR:
		av_packet_unref(&m_packet);
		return iRes == 0;
	}

	BOOL IntelQSVDecode::Close()
	{
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

	AVFrame* IntelQSVDecode::GetYUV420() const
	{
		return m_pYUV420;
	}
	AVFrame* IntelQSVDecode::GetRGB32() const
	{
		return m_pRGB32;
	}
}