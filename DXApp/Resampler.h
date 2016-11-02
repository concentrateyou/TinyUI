#pragma once
#include "Utility.h"
#include "Media/TinyWave.h"
extern "C"
{
#include "libavutil/avutil.h"
#include "libavutil/imgutils.h"
#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include <libavcodec/avcodec.h>
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
}
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "swscale.lib")
using namespace TinyUI::IO;

class Resampler
{
public:
	Resampler();
	~Resampler();
	BOOL Initialize(const WAVEFORMATEX* pInputFormat, const WAVEFORMATEX* pOutFormat);
	BOOL Resample(BYTE* bits, LONG size);
private:
	WAVEFORMATEX			m_inputFormat;
	WAVEFORMATEX			m_outFormat;
	SwrContext*				m_resample;
	TinyScopedArray<BYTE>	m_bits;
	Media::TinyWaveFile		m_waveFile;
};

