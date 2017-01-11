#include "stdafx.h"
#include "FFmpegResampler.h"
#include "Media/TinyMedia.h"

static inline uint64_t ConvertChannelLayoutToFFmpeg(const WAVEFORMATEX* pFMT)
{
	switch (Media::ConvertChannelLayout(pFMT))
	{
	case Media::CHANNEL_UNKNOWN:          return 0;
	case Media::CHANNEL_MONO:             return AV_CH_LAYOUT_MONO;
	case Media::CHANNEL_STEREO:           return AV_CH_LAYOUT_STEREO;
	case Media::CHANNEL_2POINT1:          return AV_CH_LAYOUT_2_1;
	case Media::CHANNEL_QUAD:             return AV_CH_LAYOUT_QUAD;
	case Media::CHANNEL_4POINT1:          return AV_CH_LAYOUT_4POINT1;
	case Media::CHANNEL_5POINT1:          return AV_CH_LAYOUT_5POINT1;
	case Media::CHANNEL_5POINT1_SURROUND: return AV_CH_LAYOUT_5POINT1_BACK;
	case Media::CHANNEL_7POINT1:          return AV_CH_LAYOUT_7POINT1;
	case Media::CHANNEL_7POINT1_SURROUND: return AV_CH_LAYOUT_7POINT1_WIDE_BACK;
	case Media::CHANNEL_SURROUND:         return AV_CH_LAYOUT_SURROUND;
	}
	return 0;
}

static inline enum AVSampleFormat ConvertAudioFormat(const WAVEFORMATEX* pFMT)
{
	if (pFMT->wFormatTag == WAVE_FORMAT_PCM)
	{
		switch (pFMT->wBitsPerSample)
		{
		case 8:
			return AV_SAMPLE_FMT_U8;
		case 16:
			return AV_SAMPLE_FMT_S16;
		case 32:
			return AV_SAMPLE_FMT_S32;
		}
	}
	if (pFMT->wFormatTag == WAVE_FORMAT_IEEE_FLOAT
		|| pFMT->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
	{
		return AV_SAMPLE_FMT_FLT;
	}
	return AV_SAMPLE_FMT_NONE;
}

namespace DXApp
{
	FFmpegResampler::FFmpegResampler()
		:m_context(NULL),
		m_output_size(0),
		m_input_format(AV_SAMPLE_FMT_NONE),
		m_output_format(AV_SAMPLE_FMT_NONE),
		m_input_layout(0),
		m_output_layout(0)
	{
		memset(m_input_buffer, 0, sizeof(uint8_t*)*MAX_AV_PLANES);
		memset(m_output_buffer, 0, sizeof(uint8_t*)*MAX_AV_PLANES);
	}

	FFmpegResampler::~FFmpegResampler()
	{

	}
	BOOL FFmpegResampler::Open(const WAVEFORMATEX* pFMTI, const WAVEFORMATEX* pFMTO, Callback<void(BYTE*, LONG, LPVOID)>&& callback)
	{
		ASSERT(pFMTI && pFMTO);
		m_waveFMTI = *pFMTI;
		m_waveFMTO = *pFMTO;
		m_callback = std::move(callback);
		m_output_layout = ConvertChannelLayoutToFFmpeg(pFMTO);
		m_input_layout = ConvertChannelLayoutToFFmpeg(pFMTI);
		m_input_format = ConvertAudioFormat(pFMTI);
		m_output_format = ConvertAudioFormat(pFMTO);
		m_context = swr_alloc_set_opts(NULL,
			m_output_layout,
			m_output_format,
			pFMTO->nSamplesPerSec,
			m_input_layout,
			m_input_format,
			pFMTI->nSamplesPerSec,
			0,
			NULL);
		if (!m_context)
			return FALSE;
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
		if (estimated > m_output_size)
		{
			if (m_output_buffer[0])
			{
				av_freep(&m_output_buffer[0]);
				m_output_buffer[0] = NULL;
			}
			av_samples_alloc(m_output_buffer,
				NULL,
				m_waveFMTO.nChannels,
				estimated,
				m_output_format,
				0);
			m_output_size = estimated;
		}
		m_input_buffer[0] = bits;
		int32_t size = swr_convert(m_context,
			m_output_buffer,
			m_output_size,
			(const BYTE**)m_input_buffer,
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
			m_output_buffer[0] = NULL;
		}
		return TRUE;
	}
}