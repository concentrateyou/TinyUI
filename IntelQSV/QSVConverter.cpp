#include "stdafx.h"
#include "QSVConverter.h"

namespace QSV
{
	QSVConverter::QSVConverter(const TinySize& srcSize, const TinySize& dstSize)
		:m_sws(NULL),
		m_nv12(NULL),
		m_argb(NULL),
		m_srcSize(srcSize),
		m_dstSize(dstSize),
		m_size(0)
	{
		m_nv12 = av_frame_alloc();
		m_argb = av_frame_alloc();
		m_sws = sws_getContext(srcSize.cx, srcSize.cy, AV_PIX_FMT_NV12, dstSize.cx, dstSize.cy, AV_PIX_FMT_ARGB, SWS_BILINEAR, NULL, NULL, NULL);
	}
	BOOL QSVConverter::NV12ToARGB(BYTE* Y, INT StrideY, BYTE* UV, INT StrideYV, INT Height, BYTE* out, INT linesize1)
	{
		/*if (!m_sws || !bits)
			return FALSE;
		av_image_fill_arrays(m_nv12->data, m_nv12->linesize, bits, AV_PIX_FMT_NV12, m_srcSize.cx, m_srcSize.cy, 1);
		m_size = av_image_get_buffer_size(AV_PIX_FMT_NV12, m_dstSize.cx, m_dstSize.cy, 1);
		if (m_bits.IsEmpty())
		{
			m_bits.Reset(new BYTE[m_size]);
		}
		ZeroMemory(m_bits, m_size * sizeof(BYTE));
		av_image_fill_arrays(m_argb->data, m_argb->linesize, m_bits.Ptr(), AV_PIX_FMT_ARGB, m_dstSize.cx, m_dstSize.cy, 1);*/
		BYTE * data[2] = { Y, UV };
		INT linesize[2] = { StrideY, StrideYV };
		BYTE * outData[1] = { out }; // RGB have one plane 
		INT outLinesize[1] = { linesize1 }; // RGB32 Stride
		sws_scale(m_sws, data, linesize, 0, Height, outData, outLinesize);
		return TRUE;
	}
	QSVConverter::~QSVConverter()
	{
		if (m_nv12)
		{
			av_frame_free(&m_nv12);
			m_nv12 = NULL;
		}
		if (m_argb)
		{
			av_frame_free(&m_argb);
			m_argb = NULL;
		}
		if (m_sws)
		{
			sws_freeContext(m_sws);
			m_sws = NULL;
		}
	}
}
