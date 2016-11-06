#pragma once
#include "Utility.h"
#include "Media/TinyWave.h"
extern "C"
{
#include <libavutil/avutil.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "swresample.lib")

using namespace TinyUI::IO;

#define MAX_AV_PLANES 8

static inline uint64_t ConvertSpeakerLayoutToFFmpeg(const WAVEFORMATEX* pFMT);
static inline enum AVSampleFormat ConvertAudioFormat(enum Media::AudioFormat s);

class FFmpegResampler
{
	DISALLOW_COPY_AND_ASSIGN(FFmpegResampler)
public:
	FFmpegResampler();
	~FFmpegResampler();
	BOOL Open(const WAVEFORMATEX* pFMTI, const WAVEFORMATEX* pFMTO, Callback<void(BYTE*, LONG, LPVOID)>& callback);
	BOOL Resample(BYTE* bits, LONG count);
	BOOL Close();
public:
	virtual void OnDataAvailable(BYTE* bits, LONG count, LPVOID lpParameter);
private:
	SwrContext*				m_context;
	WAVEFORMATEX			m_waveFMTI;
	WAVEFORMATEX			m_waveFMTO;
	uint64_t				m_input_layout;
	uint8_t*				m_output_buffer[MAX_AV_PLANES];
	uint8_t*				m_input_buffer[MAX_AV_PLANES];
	Callback<void(BYTE*, LONG, LPVOID)>	m_callback;
	Media::TinyWaveFile			m_waveFile;
};
