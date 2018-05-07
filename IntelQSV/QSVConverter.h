#pragma once
#include "QSVCommon.h"
#include <emmintrin.h>
#include <smmintrin.h>
#include "libyuv.h"

extern "C"
{
#include "libavutil/avutil.h"
#include "libavutil/imgutils.h"
#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
}
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swscale.lib")

namespace QSV
{
	class QSVConverter
	{
		DISALLOW_COPY_AND_ASSIGN(QSVConverter)
	public:
		QSVConverter(const TinySize& srcSize, const TinySize& dstSize);
		~QSVConverter();
		BOOL NV12ToARGB(BYTE* Y, INT StrideY,BYTE* UV, INT StrideYV, INT Height, BYTE* out, INT linesize1);
	private:
		AVFrame*				m_nv12;
		AVFrame*				m_argb;
		SwsContext*				m_sws;
		TinySize				m_srcSize;
		TinySize				m_dstSize;
		LONG					m_size;
		TinyScopedArray<BYTE>	m_bits;
	};
}
