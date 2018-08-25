#include "stdafx.h"
#include "AudioPacket.h"

namespace MediaSDK
{
	AudioPacket::AudioPacket(const AudioParameters& params)
		:m_wFrames(params.GetFrames()),
		m_wChannels(0),
		m_wBitsPerSample(0)
	{
		const WAVEFORMATEX* waveFMT = params.GetFormat();
		ASSERT(waveFMT);
		m_wChannels = waveFMT->nChannels;
		m_wBitsPerSample = waveFMT->wBitsPerSample;
		m_capacity = m_wFrames * ((m_wChannels * m_wBitsPerSample) / 8);
		m_bits.Reset(new CHAR[m_capacity]);
		ASSERT(m_bits);
	}
	AudioPacket::~AudioPacket()
	{

	}
	void AudioPacket::SetCapacity(const UINT32& capacity)
	{
		m_capacity = capacity;
		m_bits.Reset(new CHAR[m_capacity]);
		ASSERT(m_bits);
	}
	void AudioPacket::SetSize(const UINT32& size)
	{
		if (size > m_capacity)
		{
			SetCapacity(size);
		}
		m_size = size;
	}
	WORD AudioPacket::Channels() const
	{
		return m_wChannels;
	}
	WORD AudioPacket::BitsPerSample() const
	{
		return m_wBitsPerSample;
	}
	WORD  AudioPacket::Frames() const
	{
		return m_wFrames;
	}
	UINT32 AudioPacket::size() const
	{
		return m_size;
	}
	UINT32 AudioPacket::capacity() const
	{
		return m_capacity;
	}
	CHAR* AudioPacket::data()
	{
		return m_bits;
	}
}