#pragma once
#include "MediaSDK.h"

namespace MediaSDK
{
	class AudioParameters
	{
	public:
		AudioParameters();
		AudioParameters(const AudioParameters& params);
		AudioParameters(AudioParameters&& params);
		AudioParameters& operator=(const AudioParameters& params);
		~AudioParameters();
		void SetFrames(WORD wFrames);
		WORD GetFrames() const;
		void SetFormat(const WAVEFORMATEX* pFMT);
		const WAVEFORMATEX* GetFormat() const;
	private:
		WORD					m_wFrames;
		TinyScopedArray<BYTE>	m_waveFMT;
	};
}