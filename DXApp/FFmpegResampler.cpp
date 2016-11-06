#include "stdafx.h"
#include "FFmpegResampler.h"
#include "Media/TinyMedia.h"

static inline uint64_t ConvertSpeakerLayoutToFFmpeg(const WAVEFORMATEX* pFMT)
{
	switch (Media::ConvertSpeakerLayout(pFMT))
	{
	case Media::SPEAKERS_UNKNOWN:          return 0;
	case Media::SPEAKERS_MONO:             return AV_CH_LAYOUT_MONO;
	case Media::SPEAKERS_STEREO:           return AV_CH_LAYOUT_STEREO;
	case Media::SPEAKERS_2POINT1:          return AV_CH_LAYOUT_2_1;
	case Media::SPEAKERS_QUAD:             return AV_CH_LAYOUT_QUAD;
	case Media::SPEAKERS_4POINT1:          return AV_CH_LAYOUT_4POINT1;
	case Media::SPEAKERS_5POINT1:          return AV_CH_LAYOUT_5POINT1;
	case Media::SPEAKERS_5POINT1_SURROUND: return AV_CH_LAYOUT_5POINT1_BACK;
	case Media::SPEAKERS_7POINT1:          return AV_CH_LAYOUT_7POINT1;
	case Media::SPEAKERS_7POINT1_SURROUND: return AV_CH_LAYOUT_7POINT1_WIDE_BACK;
	case Media::SPEAKERS_SURROUND:         return AV_CH_LAYOUT_SURROUND;
	}
	return 0;
}

static inline enum AVSampleFormat ConvertAudioFormat(enum Media::AudioFormat s)
{
	switch (s)
	{
	case Media::AUDIO_FORMAT_UNKNOWN:      return AV_SAMPLE_FMT_S16;
	case Media::AUDIO_FORMAT_U8BIT:        return AV_SAMPLE_FMT_U8;
	case Media::AUDIO_FORMAT_16BIT:        return AV_SAMPLE_FMT_S16;
	case Media::AUDIO_FORMAT_32BIT:        return AV_SAMPLE_FMT_S32;
	case Media::AUDIO_FORMAT_FLOAT:        return AV_SAMPLE_FMT_FLT;
	case Media::AUDIO_FORMAT_U8BIT_PLANAR: return AV_SAMPLE_FMT_U8P;
	case Media::AUDIO_FORMAT_16BIT_PLANAR: return AV_SAMPLE_FMT_S16P;
	case Media::AUDIO_FORMAT_32BIT_PLANAR: return AV_SAMPLE_FMT_S32P;
	case Media::AUDIO_FORMAT_FLOAT_PLANAR: return AV_SAMPLE_FMT_FLTP;
	}
	return AV_SAMPLE_FMT_S16;
}


FFmpegResampler::FFmpegResampler()
	:m_context(NULL)
{

}

FFmpegResampler::~FFmpegResampler()
{

}
BOOL FFmpegResampler::Open(const WAVEFORMATEX* pFMTI, const WAVEFORMATEX* pFMTO, Callback<void(BYTE*, LONG, LPVOID)>& callback)
{
	ASSERT(pFMTI && pFMTO);
	m_waveFMTI = *pFMTI;
	m_waveFMTO = *pFMTO;
	m_callback = callback;
	uint64_t output_layout = ConvertSpeakerLayoutToFFmpeg(pFMTO);
	uint64_t input_layout = ConvertSpeakerLayoutToFFmpeg(pFMTI);
	m_context = swr_alloc_set_opts(NULL,
		output_layout,
		AV_SAMPLE_FMT_S16,
		pFMTO->nSamplesPerSec,
		input_layout,
		AV_SAMPLE_FMT_FLT,
		pFMTI->nSamplesPerSec,
		0,
		NULL);
	return swr_init(m_context) == 0;
}
BOOL FFmpegResampler::Resample(BYTE* bits, LONG count)
{
	if (!m_context)
		return FALSE;
	int64_t delay = swr_get_delay(m_context, m_waveFMTI.nSamplesPerSec);
	int32_t estimated = (int)av_rescale_rnd(
		delay + (int64_t)count,
		(int64_t)m_waveFMTO.nSamplesPerSec,
		(int64_t)m_waveFMTI.nSamplesPerSec,
		AV_ROUND_UP);
	if (m_output_buffer[0])
	{
		av_freep(&m_output_buffer[0]);
	}
	av_samples_alloc(m_output_buffer,
		NULL,
		m_waveFMTO.nChannels,
		estimated,
		AV_SAMPLE_FMT_S16,
		0);
	m_input_buffer[0] = bits;
	int size = swr_convert(m_context,
		m_output_buffer,
		estimated,
		(const uint8_t**)m_input_buffer,
		count);
	if (size && m_output_buffer[0])
	{
		OnDataAvailable(m_output_buffer[0], size, this);
		return TRUE;
	}
	return FALSE;
}

void FFmpegResampler::OnDataAvailable(BYTE* bits, LONG count, LPVOID lpParameter)
{
	if (!m_callback.IsNull())
	{
		m_callback(bits, count, lpParameter);
	}
}

BOOL FFmpegResampler::Close()
{
	if (m_output_buffer[0])
	{
		av_freep(&m_output_buffer[0]);
	}
	m_waveFile.Close();
	return TRUE;
}
