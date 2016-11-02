#include "stdafx.h"
#include "Resampler.h"


Resampler::Resampler()
	:m_resample(NULL)
{
}


Resampler::~Resampler()
{

}

BOOL Resampler::Initialize(const WAVEFORMATEX* pInputFormat, const WAVEFORMATEX* pOutFormat)
{
	ASSERT(pInputFormat && pOutFormat);
	m_inputFormat = *pInputFormat;
	m_outFormat = *pOutFormat;
	//m_waveFile.Create("D:\\abc.wav", pInputFormat);
	AVSampleFormat sampleIN = AV_SAMPLE_FMT_NONE;
	switch (pInputFormat->wBitsPerSample)
	{
	case 16:
	case 24:
		sampleIN = AV_SAMPLE_FMT_S16;
		break;
	case 32:
		sampleIN = AV_SAMPLE_FMT_S32;
		break;
	}
	AVSampleFormat sampleOUT = AV_SAMPLE_FMT_NONE;
	switch (pOutFormat->wBitsPerSample)
	{
	case 16:
	case 24:
		sampleOUT = AV_SAMPLE_FMT_S16;
		break;
	case 32:
		sampleOUT = AV_SAMPLE_FMT_S32;
		break;
	}
	int64_t out_ch_layout = av_get_default_channel_layout(pOutFormat->nChannels);
	int64_t in_ch_layout = av_get_default_channel_layout(pInputFormat->nChannels);
	m_resample = swr_alloc_set_opts(NULL, out_ch_layout, sampleOUT, pOutFormat->nSamplesPerSec, in_ch_layout, sampleIN, pInputFormat->nSamplesPerSec, 0, NULL);
	int s = swr_init(m_resample);
	return TRUE;
}

BOOL Resampler::Resample(BYTE* bits, LONG size)
{
	//m_waveFile.Write(bits, size);
	AVSampleFormat sampleOUT = AV_SAMPLE_FMT_NONE;
	switch (m_outFormat.wBitsPerSample)
	{
	case 16:
	case 24:
		sampleOUT = AV_SAMPLE_FMT_S16;
		break;
	case 32:
		sampleOUT = AV_SAMPLE_FMT_FLT;
		break;
	}
	int32_t nb_samples = size / m_inputFormat.nBlockAlign;
	int  out_samples = av_rescale_rnd(nb_samples, m_outFormat.nSamplesPerSec, m_inputFormat.nSamplesPerSec, AV_ROUND_UP);
	int output_data_size = av_samples_get_buffer_size(NULL, m_outFormat.nChannels, out_samples, sampleOUT, 1);
	unsigned int buf_size = 0;
	uint8_t* buffer = new uint8_t[10240];
	out_samples = swr_convert(m_resample, (uint8_t **)&buffer, out_samples, (const uint8_t **)bits, nb_samples);
	av_freep(buffer);
	return TRUE;

}