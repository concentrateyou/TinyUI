#include "stdafx.h"
#include "AudioPacket.h"

namespace MediaSDK
{
	AudioPacket::AudioPacket(UINT32 size)
		:m_size(size)
	{
		m_bits.Reset(new CHAR[m_size]);
		ASSERT(m_bits);
	}

	AudioPacket::~AudioPacket()
	{
	}
	CHAR*	AudioPacket::data()
	{
		return m_bits;
	}
}