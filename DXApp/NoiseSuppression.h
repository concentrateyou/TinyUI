#pragma once
#include "Utility.h"
#include "Media/TinyWave.h"
extern "C"
{
#include "signal_processing_library.h"
#include "noise_suppression_x.h"
#include "noise_suppression.h"
#include "gain_control.h"
}
#pragma comment(lib, "WebRTCNS.lib")

/// <summary>
/// WebRTC½µÔëÄ£¿é
/// </summary>
class NoiseSuppression
{
	DISALLOW_COPY_AND_ASSIGN(NoiseSuppression)
public:
	NoiseSuppression();
	~NoiseSuppression();
	BOOL Open(const WAVEFORMATEX* pFMT);
	BOOL Denoise(BYTE* bits, LONG count);
	BOOL Close();
protected:
	NsHandle*				m_pNS;
	WAVEFORMATEX			m_waveFMT;
	TinyScopedArray<BYTE>	m_bits;
};

