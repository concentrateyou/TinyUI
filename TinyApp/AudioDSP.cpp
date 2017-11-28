#include "stdafx.h"
#include "AudioDSP.h"


AudioDSP::AudioDSP()
{
}


AudioDSP::~AudioDSP()
{
}

BOOL AudioDSP::Initialize()
{
	m_audioDSP.Initialize(BindCallback(&AudioDSP::OnDSP, this));
	vector<TinyWASAPIAudio::Name> captureNames;
	TinyWASAPIAudio::GetDevices(eCapture, captureNames);
	vector<TinyWASAPIAudio::Name> renderNames;
	TinyWASAPIAudio::GetDevices(eRender, renderNames);
	TinyWASAPIAudio::Name captureName;
	for (INT i = 0;i < captureNames.size();i++)
	{
		BOOL IsMA = FALSE;
		TinyWASAPIAudio::IsMicrophone(captureNames[i], IsMA);
		if (IsMA)
		{
			captureName = captureNames[i];
			break;
		}
	}
	WAVEFORMATEX waveFMTI;
	ZeroMemory(&waveFMTI, sizeof(waveFMTI));
	waveFMTI.cbSize = 0;
	waveFMTI.wFormatTag = WAVE_FORMAT_PCM;
	waveFMTI.nChannels = 1;
	waveFMTI.nSamplesPerSec = 16000;
	waveFMTI.wBitsPerSample = 16;
	waveFMTI.nBlockAlign = 2;
	waveFMTI.nAvgBytesPerSec = 32000;
	WAVEFORMATEX waveFMTO;
	ZeroMemory(&waveFMTO, sizeof(waveFMTO));
	waveFMTO.cbSize = 0;
	waveFMTO.wFormatTag = WAVE_FORMAT_PCM;
	waveFMTO.nChannels = 2;
	waveFMTO.nSamplesPerSec = 44100;
	waveFMTO.wBitsPerSample = 16;
	waveFMTO.nBlockAlign = 4;
	waveFMTO.nAvgBytesPerSec = 176400;
	m_timer.SetCallback(24, BindCallback(&AudioDSP::OnTimer, this));
	m_audioDSP.EnableNS(TRUE);
	m_audioDSP.EnableAGC(FALSE);
	m_audioDSP.Open(renderNames[0], captureName, &waveFMTI);
	m_waveFile.Create("D:\\12345.wav", &waveFMTO);
	m_resampler.Open(&waveFMTI, &waveFMTO, BindCallback(&AudioDSP::OnAudio, this));
	m_audioDSP.Start();
	m_timeQPC.BeginTime();
	return TRUE;
}

void AudioDSP::OnDSP(BYTE* bits, LONG size, LPVOID lpParameter)
{
	m_resampler.Resample(bits, size);
}
void AudioDSP::OnAudio(BYTE* bits, LONG size, LPVOID lpParameter)
{
	TinyAutoLock lock(m_lock);
	m_buffer.Add(bits, size);
}
void AudioDSP::OnTimer()
{
	TinyAutoLock lock(m_lock);
	if (m_buffer.GetSize() >= 4096)
	{
		memcpy(m_data, m_buffer.GetPointer(), 4096);
		m_buffer.Remove(0, 4096);
		m_timeQPC.EndTime();
		m_waveFile.Write(m_data, 4096);
		m_timeQPC.BeginTime();
		TRACE("OnTimer:%lld\n", m_timeQPC.GetMillisconds());
	}
	else
	{
		TRACE("OnTimer: Buffer < 4096 - %d\n", m_buffer.GetSize());
	}
}
