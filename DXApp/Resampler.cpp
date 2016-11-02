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
	AVSampleFormat sampleIN = AV_SAMPLE_FMT_NONE;
	switch (pInputFormat->wBitsPerSample)
	{
	case 16:
	case 24:
		sampleIN = AV_SAMPLE_FMT_S16;
		break;
	case 32:
		sampleIN = AV_SAMPLE_FMT_FLT;
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
		sampleOUT = AV_SAMPLE_FMT_FLT;
		break;
	}

	return TRUE;
}

BOOL Resampler::Resample(BYTE* bits, LONG size)
{
	return TRUE;
}