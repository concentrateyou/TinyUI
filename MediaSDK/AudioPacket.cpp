#include "stdafx.h"
#include "AudioPacket.h"

namespace MediaSDK
{
	AudioPacket::AudioPacket(const AudioParameters& params)
		:m_wCount(params.GetFrames()),
		m_wChannels(0),
		m_wBitsPerSample(0)
	{
		const WAVEFORMATEX* waveFMT = params.GetFormat();
		ASSERT(waveFMT);
		m_wChannels = waveFMT->nChannels;
		m_wBitsPerSample = waveFMT->wBitsPerSample;
		m_size = m_wCount * ((m_wChannels * m_wBitsPerSample) / 8);
		m_bits.Reset(new CHAR[m_size]);
		ASSERT(m_bits);
	}
	AudioPacket::~AudioPacket()
	{

	}
	void AudioPacket::Reset(const UINT32& size)
	{
		m_size = size;
		m_bits.Reset(new CHAR[m_size]);
		ASSERT(m_bits);
	}
	WORD AudioPacket::Channels() const
	{
		return m_wChannels;
	}
	WORD AudioPacket::BitsPerSample() const
	{
		return m_wBitsPerSample;
	}
	WORD  AudioPacket::count() const
	{
		return m_wCount;
	}
	UINT32 AudioPacket::size() const
	{
		return m_size;
	}
	CHAR* AudioPacket::data()
	{
		return m_bits;
	}
}