#include "stdafx.h"
#include "AudioPacket.h"

namespace MediaSDK
{
	AudioPacket::AudioPacket(UINT32 channels, UINT32 count)
		:m_count(count)
	{
		count = ((count * sizeof(FLOAT) + 15) &~(15)) / sizeof(FLOAT);
		INT size = sizeof(FLOAT) * channels * count;
		m_bits.Reset(static_cast<FLOAT*>(_aligned_malloc(size, 16)));
		ASSERT(m_bits);
		m_channels.reserve(channels);
		for (UINT32 i = 0; i < channels; ++i)
		{
			m_channels.push_back(m_bits + i * count);
		}
	}

	AudioPacket::~AudioPacket()
	{
	}
	void AudioPacket::Copy(UINT32 channels, FLOAT* data)
	{

	}
	void AudioPacket::Scale(FLOAT volume)
	{
		do
		{
			if (volume == 0)
			{
				if (m_count <= 0)
					break;
				for (size_t i = 0; i < m_channels.size(); ++i)
				{
					memset(m_channels[i], 0, m_count * sizeof(*m_channels[i]));
				}
				break;
			}
			if (volume > 0 && volume != 1)
			{
				for (size_t i = 0; i < m_channels.size(); ++i)
				{
					//vector_math::FMUL(channel(i), volume, m_count, channel(i));
				}
			}
		} while (0);
	}
}