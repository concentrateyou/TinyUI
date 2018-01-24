#include "stdafx.h"
#include "H264BitReader.h"

namespace Decode
{
	H264BitReader::H264BitReader()
		: m_bits(NULL),
		m_remainingBytes(0),
		m_currentByte(0),
		m_remainingBits(0),
		m_prev2Bytes(0),
		m_emulationPreventionBytes(0)
	{
	}

	H264BitReader::~H264BitReader()
	{
	}

	BOOL H264BitReader::Initialize(const BYTE* bits, off_t size)
	{
		if (!bits || size <= 0)
			return FALSE;
		m_bits = bits;
		m_remainingBytes = size;
		m_remainingBits = 0;
		m_prev2Bytes = 0xFFFF;
		m_emulationPreventionBytes = 0;
		return TRUE;
	}

	BOOL H264BitReader::UpdateCurrentByte()
	{
		if (m_remainingBytes < 1)
			return FALSE;
		if (*m_bits == 0x03 && (m_prev2Bytes & 0xFF) == 0)
		{
			++m_bits;
			--m_remainingBytes;
			++m_emulationPreventionBytes;
			m_prev2Bytes = 0xFFFF;
			if (m_remainingBytes < 1)
				return FALSE;
		}
		m_currentByte = *m_bits++ & 0xFF;
		--m_remainingBytes;
		m_remainingBits = 8;
		m_prev2Bytes = ((m_prev2Bytes & 0xFF) << 8) | m_currentByte;
		return TRUE;
	}

	BOOL H264BitReader::ReadBits(INT count, INT32* val)
	{
		*val = 0;
		if (count > 31)
			return FALSE;
		INT remainingBits = count;
		while (m_remainingBits < remainingBits)
		{
			*val |= (m_currentByte << (remainingBits - m_remainingBits));
			remainingBits -= m_remainingBits;
			if (!UpdateCurrentByte())
				return FALSE;
		}
		*val |= (m_currentByte >> (m_remainingBits - remainingBits));
		*val &= ((1u << count) - 1u);
		m_remainingBits -= remainingBits;
		return TRUE;
	}

	off_t H264BitReader::Available()
	{
		return (m_remainingBits + m_remainingBytes * 8);
	}

	BOOL H264BitReader::HasRBSP()
	{
		if (m_remainingBits == 0 && !UpdateCurrentByte())
			return FALSE;
		if ((m_currentByte & ((1 << (m_remainingBits - 1)) - 1)) != 0)
			return TRUE;
		for (off_t i = 0; i < m_remainingBytes; i++)
		{
			if (m_bits[i] != 0)
				return TRUE;
		}
		m_remainingBytes = 0;
		return FALSE;
	}

	size_t H264BitReader::EmulationPreventionBytes()
	{
		return m_emulationPreventionBytes;
	}
}