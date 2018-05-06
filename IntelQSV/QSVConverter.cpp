#include "stdafx.h"
#include "QSVConverter.h"

namespace QSV
{
	INT QSVConverter::ConverterNV12ToRGB32(const uint8_t* src_y,
		int src_stride_y,
		const uint8_t* src_uv,
		int src_stride_uv,
		uint8_t* dst_argb,
		int dst_stride_argb,
		int width,
		int height)
	{
		return	libyuv::NV12ToARGB(src_y, src_stride_y, src_uv, src_stride_uv, dst_argb, dst_stride_argb, width, height);
	}
}
