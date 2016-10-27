#include "stdafx.h"
#include "AudioCaptureParam.h"

namespace DShow
{
	AudioCaptureParam::AudioCaptureParam()
	{
	}

	AudioCaptureParam::~AudioCaptureParam()
	{
	}
	WAVEFORMATEX AudioCaptureParam::GetFormat() const
	{
		return m_wft;
	}
	void AudioCaptureParam::SetFormat(const WAVEFORMATEX& wfx)
	{
		m_wft = wfx;
	}
	string AudioCaptureParam::ToString() const
	{
		return StringPrintf("%d,%d,%d,%d,%d", m_wft.nAvgBytesPerSec, m_wft.nBlockAlign, m_wft.nChannels, m_wft.nSamplesPerSec, m_wft.wBitsPerSample);
	}
	WAVEFORMATEX AudioCaptureParam::GetDefaultFormat()
	{
		WAVEFORMATEX w;
		w.cbSize = sizeof(w);
		w.nChannels = 2;
		w.nSamplesPerSec = 44100;
		w.wBitsPerSample = 16;
		w.nBlockAlign = (w.wBitsPerSample * w.nChannels) / 8;
		w.nAvgBytesPerSec = w.nSamplesPerSec * w.nBlockAlign;
		w.wFormatTag = WAVE_FORMAT_PCM;
		return w;
	}
}