#pragma once
#include "QSVCommon.h"
#include <emmintrin.h>
#include <smmintrin.h>
#include "libyuv.h"

namespace QSV
{
	class QSVConverter
	{
		DISALLOW_COPY_AND_ASSIGN(QSVConverter)
	public:
		INT ConverterNV12ToRGB32(const uint8_t* src_y,
			int src_stride_y,
			const uint8_t* src_uv,
			int src_stride_uv,
			uint8_t* dst_argb,
			int dst_stride_argb,
			int width,
			int height);
	};
}
