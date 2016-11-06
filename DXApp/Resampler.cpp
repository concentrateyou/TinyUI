#include "stdafx.h"
#include "Resampler.h"


Resampler::Resampler()
	:m_resampler(NULL)
{
}


Resampler::~Resampler()
{

}

BOOL Resampler::Open(const WAVEFORMATEX* pFMTI, const WAVEFORMATEX* pFMTO, Callback<void(FLOAT*, LONG, LPVOID)>& callback)
{
	ASSERT(pFMTI && pFMTO);
	m_pFMTI = pFMTI;
	m_pFMTO = pFMTO;
	m_callback = callback;
	INT converterType = SRC_SINC_BEST_QUALITY;
	INT iRes = 0;
	m_resampler = src_new(converterType, 2, &iRes);
	if (iRes != 0)
		return FALSE;
	m_resampleRatio = DOUBLE(pFMTO->nSamplesPerSec) / DOUBLE(pFMTI->nSamplesPerSec);
	m_dwBytesPerSample = (pFMTI->nChannels * pFMTI->wBitsPerSample) / 8;
	SRC_DATA data;
	data.src_ratio = m_resampleRatio;
	TinyScopedArray<FLOAT> blank(new FLOAT[pFMTI->nSamplesPerSec / 100 * 2]);
	data.data_in = blank;
	data.input_frames = pFMTI->nSamplesPerSec / 100;
	UINT adjust = UINT((DOUBLE(data.input_frames) * m_resampleRatio) + 1.0);
	UINT newSize = adjust * 2;
	TinyScopedArray<FLOAT> blank2(new FLOAT[newSize]);
	data.data_out = blank2;
	data.output_frames = adjust;
	data.end_of_input = 0;
	return src_process(m_resampler, &data) == 0;
}

BOOL Resampler::Resample(BYTE* bits, LONG count)
{
	ASSERT(m_resampler);
	if (m_size != count * m_dwBytesPerSample)
	{
		m_bits.Reset(new FLOAT[count * m_dwBytesPerSample]);
	}
	memcpy(m_bits, (FLOAT*)bits, count * m_dwBytesPerSample);
	UINT outputFrame = UINT((DOUBLE(count) * m_resampleRatio) + 1.0);
	TinyScopedArray<FLOAT> output(new FLOAT[outputFrame * 2]);
	SRC_DATA data;
	data.src_ratio = m_resampleRatio;
	data.data_in = m_bits;
	data.input_frames = count;
	data.data_out = output;
	data.output_frames = outputFrame;
	data.end_of_input = 0;
	if (src_process(m_resampler, &data) != 0)
		return FALSE;
	OnDataAvailable(output, data.output_frames_gen * 2, this);
	return TRUE;
}
BOOL Resampler::Close()
{
	return TRUE;
}
void Resampler::OnDataAvailable(FLOAT* bits, LONG size, LPVOID lpParameter)
{
	if (!m_callback.IsNull())
	{
		m_callback(bits, size, lpParameter);
	}
}