#include "stdafx.h"
#include "AudioParameters.h"
#include "MediaSDK.h"

namespace MediaSDK
{
	AudioParameters::AudioParameters()
	{
		m_waveFMT.Reset(new BYTE[sizeof(WAVEFORMATEX)]);
		ASSERT(m_waveFMT);
		ZeroMemory(m_waveFMT, sizeof(WAVEFORMATEX));
		WAVEFORMATEX* pFMT = reinterpret_cast<WAVEFORMATEX*>(m_waveFMT.Ptr());
		pFMT->nChannels = 2;
		pFMT->nSamplesPerSec = 44100;
		pFMT->wBitsPerSample = 16;
		pFMT->nBlockAlign = (pFMT->wBitsPerSample * pFMT->nChannels) / 8;
		pFMT->nAvgBytesPerSec = pFMT->nSamplesPerSec * pFMT->nBlockAlign;
		pFMT->wFormatTag = WAVE_FORMAT_PCM;
	}

	AudioParameters::~AudioParameters()
	{
	}
	AudioParameters::AudioParameters(const AudioParameters& params)
	{
		m_wFrames = params.m_wFrames;
		const WAVEFORMATEX* pFMT = params.GetFormat();
		ASSERT(pFMT);
		INT size = sizeof(WAVEFORMATEX) + (pFMT != NULL ? pFMT->cbSize : 0);
		if (params.m_waveFMT != NULL)
		{
			m_waveFMT.Reset(new BYTE[size]);
			ASSERT(m_waveFMT);
			ZeroMemory(m_waveFMT, size);
			memcpy_s(m_waveFMT, size, params.m_waveFMT, size);
		}
	}
	AudioParameters::AudioParameters(AudioParameters&& params)
		:m_waveFMT(std::move(params.m_waveFMT)),
		m_wFrames(params.m_wFrames)
	{
		params.m_wFrames = 0;
	}
	AudioParameters& AudioParameters::operator=(const AudioParameters& params)
	{
		if (&params != this)
		{
			m_wFrames = params.m_wFrames;
			const WAVEFORMATEX* pFMT = params.GetFormat();
			ASSERT(pFMT);
			INT size = sizeof(WAVEFORMATEX) + (pFMT != NULL ? pFMT->cbSize : 0);
			m_waveFMT.Reset(new BYTE[size]);
			ASSERT(m_waveFMT);
			ZeroMemory(m_waveFMT, sizeof(WAVEFORMATEX));
			if (params.m_waveFMT != NULL)
			{
				memcpy_s(m_waveFMT, size, params.m_waveFMT, size);
			}
		}
		return *this;
	}
	void AudioParameters::SetFrames(WORD wFrames)
	{
		m_wFrames = wFrames;
	}
	WORD AudioParameters::GetFrames() const
	{
		return m_wFrames;
	}
	const WAVEFORMATEX* AudioParameters::GetFormat() const
	{
		return reinterpret_cast<WAVEFORMATEX*>(m_waveFMT.Ptr());
	}
	void AudioParameters::SetFormat(const WAVEFORMATEX* pFMT)
	{
		if (pFMT != NULL)
		{
			INT size = sizeof(WAVEFORMATEX) + pFMT->cbSize;
			m_waveFMT.Reset(new BYTE[size]);
			ASSERT(m_waveFMT);
			ZeroMemory(m_waveFMT, sizeof(WAVEFORMATEX));
			memcpy_s(m_waveFMT, size, pFMT, size);
		}
	}
}