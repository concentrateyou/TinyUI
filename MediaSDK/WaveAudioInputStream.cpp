#include "stdafx.h"
#include "WaveAudioInputStream.h"

namespace MediaSDK
{
	WaveAudioInputStream::WaveAudioInputStream()
		:m_dID(0),
		m_count(0),
		m_size(0),
		m_alignsize(0),
		m_state(PCM_NONE),
		m_callback(NULL)
	{
	}

	WaveAudioInputStream::~WaveAudioInputStream()
	{
	}

	BOOL WaveAudioInputStream::Initialize(const AudioParameters& params, UINT count, UINT dID)
	{
		const WAVEFORMATEX* pFMT = params.GetFormat();
		m_waveFMT.wFormatTag = WAVE_FORMAT_PCM;
		m_waveFMT.nChannels = params.() > 2 ? 2 : params.channels();
		m_waveFMT.nSamplesPerSec = params.sample_rate();
		m_waveFMT.wBitsPerSample = params.bits_per_sample();
		m_waveFMT.cbSize = 0;
		m_waveFMT.nBlockAlign = (m_waveFMT.nChannels * m_waveFMT.wBitsPerSample) / 8;
		m_waveFMT.nAvgBytesPerSec = m_waveFMT.nBlockAlign * m_waveFMT.nSamplesPerSec;
	}

	inline WAVEHDR* WaveAudioInputStream::GetWAVEHDR(INT index) const
	{
		return reinterpret_cast<WAVEHDR*>(&m_bits[index * m_alignsize]);
	}

	BOOL WaveAudioInputStream::Open()
	{
		return TRUE;
	}


	BOOL WaveAudioInputStream::Start(AudioOutputCallback* callback)
	{
		return TRUE;
	}

	BOOL WaveAudioInputStream::Stop()
	{
		return TRUE;
	}

	void WaveAudioInputStream::Close()
	{

	}

	BOOL WaveAudioInputStream::SetVolume(DOUBLE volume)
	{
		return TRUE;
	}

	BOOL WaveAudioInputStream::GetVolume(DOUBLE* volume)
	{
		return TRUE;
	}

}