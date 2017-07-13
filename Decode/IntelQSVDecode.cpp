#include "stdafx.h"
#include "IntelQSVDecode.h"

namespace Decode
{
	static int get_format(AVCodecContext *avctx, const enum AVPixelFormat *pix_fmts)
	{
		while (*pix_fmts != AV_PIX_FMT_NONE) {
			if (*pix_fmts == AV_PIX_FMT_QSV) {
				DecodeContext *decode = avctx->opaque;
				AVHWFramesContext  *frames_ctx;
				AVQSVFramesContext *frames_hwctx;
				int ret;

				/* create a pool of surfaces to be used by the decoder */
				avctx->hw_frames_ctx = av_hwframe_ctx_alloc(decode->hw_device_ref);
				if (!avctx->hw_frames_ctx)
					return AV_PIX_FMT_NONE;
				frames_ctx = (AVHWFramesContext*)avctx->hw_frames_ctx->data;
				frames_hwctx = frames_ctx->hwctx;

				frames_ctx->format = AV_PIX_FMT_QSV;
				frames_ctx->sw_format = avctx->sw_pix_fmt;
				frames_ctx->width = FFALIGN(avctx->coded_width, 32);
				frames_ctx->height = FFALIGN(avctx->coded_height, 32);
				frames_ctx->initial_pool_size = 32;

				frames_hwctx->frame_type = MFX_MEMTYPE_VIDEO_MEMORY_DECODER_TARGET;

				ret = av_hwframe_ctx_init(avctx->hw_frames_ctx);
				if (ret < 0)
					return AV_PIX_FMT_NONE;

				return AV_PIX_FMT_QSV;
			}

			pix_fmts++;
		}

		fprintf(stderr, "The QSV pixel format not offered in get_format()\n");

		return AV_PIX_FMT_NONE;
	}
	IntelQSVDecode::IntelQSVDecode()
		:m_codec(NULL),
		m_bufferRef(NULL),
		m_context(NULL)
	{
	}


	IntelQSVDecode::~IntelQSVDecode()
	{
	}

	BOOL IntelQSVDecode::Initialize(const TinySize& srcsize, const TinySize& dstsize)
	{
		avcodec_register_all();
		av_init_packet(&m_packet);
		INT iRes = av_hwdevice_ctx_create(&m_bufferRef, AV_HWDEVICE_TYPE_QSV, "auto", NULL, 0);
		if (iRes < 0)
			goto _ERROR;
		m_codec = avcodec_find_decoder("h264_qsv");
		if (!m_codec)
			goto _ERROR;
		m_context = avcodec_alloc_context3(m_codec);
		if (!m_context)
			goto _ERROR;
		m_context->codec_id = AV_CODEC_ID_H264;
		return TRUE;
	_ERROR:
		Close();
		return FALSE;
	}

	BOOL IntelQSVDecode::Open(BYTE* metadata, LONG size)
	{

	}
	BOOL IntelQSVDecode::Close()
	{
		if (m_bufferRef != NULL)
		{
			av_buffer_unref(&m_bufferRef);
		}
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
		return TRUE;
	}
}