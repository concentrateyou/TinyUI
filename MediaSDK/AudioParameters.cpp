#include "stdafx.h"
#include "AudioParameters.h"
#include "MediaSDK.h"

namespace MediaSDK
{
	AudioParameters::AudioParameters()
	{
		m_waveFMT.Reset(new WAVEFORMATEX());
		ASSERT(m_waveFMT);
		ZeroMemory(&m_waveFMT, sizeof(WAVEFORMATEX));
	}


	AudioParameters::~AudioParameters()
	{
	}
	AudioParameters::AudioParameters(const AudioParameters& params)
	{
		m_wFrames = params.m_wFrames;
		INT size = sizeof(WAVEFORMATEX) + (params.m_waveFMT != NULL ? params.m_waveFMT->cbSize : 0);
		BYTE* bits = new BYTE[size];
		ASSERT(bits);
		m_waveFMT.Reset(reinterpret_cast<WAVEFORMATEX*>(bits));
		ZeroMemory(m_waveFMT, sizeof(WAVEFORMATEX));
		if (params.m_waveFMT != NULL)
		{
			memcpy_s(m_waveFMT, size, params.m_waveFMT, size);
		}
	}
	AudioParameters::AudioParameters(AudioParameters&& params)
		:m_waveFMT(std::move(params.m_waveFMT)),
		m_wFrames(params.m_wFrames)
	{
		params.m_wFrames = 0;
		params.m_waveFMT.Release();
	}
	AudioParameters& AudioParameters::operator=(const AudioParameters& params)
	{
		if (&params != this)
		{
			m_wFrames = params.m_wFrames;
			INT size = sizeof(WAVEFORMATEX) + (params.m_waveFMT != NULL ? params.m_waveFMT->cbSize : 0);
			BYTE* bits = new BYTE[size];
			ASSERT(bits);
			m_waveFMT.Reset(reinterpret_cast<WAVEFORMATEX*>(bits));
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
		return m_waveFMT;
	}
	void AudioParameters::SetFormat(WAVEFORMATEX* pFMT)
	{
		if (pFMT != NULL && m_waveFMT != pFMT)
		{
			INT size = sizeof(WAVEFORMATEX) + pFMT->cbSize;
			BYTE* bits = new BYTE[size];
			ASSERT(bits);
			m_waveFMT.Reset(reinterpret_cast<WAVEFORMATEX*>(bits));
			ZeroMemory(m_waveFMT, sizeof(WAVEFORMATEX));
			memcpy_s(m_waveFMT, size, pFMT, size);
		}
	}
}