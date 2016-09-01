#pragma once
#include <stdio.h>
#include <stdint.h>
#include "libavutil/avutil.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavcodec/avcodec.h"
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "swscale.lib")

class I420Converter
{
public:
	I420Converter();
	~I420Converter();
	BOOL BRGAToI420();
private:
	AVFrame*	m_yuv;
	AVFrame*	m_bgra;
};

