#include "stdafx.h"
#include "MAudioAnalyser.h"

namespace MShow
{
	MAudioAnalyser::MAudioAnalyser()
	{
	}

	MAudioAnalyser::~MAudioAnalyser()
	{
	}
	BOOL MAudioAnalyser::Initialize(HWND hWND, WAVEFORMATEX& waveFMT)
	{
		if (!m_wFFT.Initialize(DEFAULT_AUDIO_SAMPLE_SIZE))
			return FALSE;
		TinyRectangle s;
		::GetClientRect(hWND, &s);
		m_size = s.Size();
		m_samplesL.Reset(new FLOAT[DEFAULT_AUDIO_SAMPLE_SIZE]);
		if (!m_samplesL)
			return FALSE;
		m_samplesR.Reset(new FLOAT[DEFAULT_AUDIO_SAMPLE_SIZE]);
		if (!m_samplesR)
			return FALSE;
		m_waveFMT = waveFMT;
		return TRUE;
	}
	void MAudioAnalyser::Process(BYTE* bits, LONG size)//4096
	{
		INT samples = size / m_waveFMT.wBitsPerSample * 8;//2048
		INT sampless = samples / 2;//单个声道1024个采样
		ASSERT(DEFAULT_AUDIO_SAMPLE_SIZE == sampless);
		INT offset = 0;
		for (INT i = 0;i < sampless;i++)
		{
			m_samplesL[i] = (FLOAT)((bits[offset + 1] << 8) + bits[offset]) / 32767.0F;
			m_samplesR[i] = (FLOAT)((bits[offset + 3] << 8) + bits[offset + 2]) / 32767.0F;
			offset += 4;
		}
		for (INT i = 0; i < sampless; i++)
		{
			m_samplesL[i] = (m_samplesL[i] + m_samplesR[i]) / 2.0F;
		}
		FLOAT* wFFT = m_wFFT.Calculate(m_samplesL, samples);
	}
}

