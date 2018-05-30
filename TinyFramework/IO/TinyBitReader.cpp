#include "../stdafx.h"
#include "TinyBitReader.h"
#include <algorithm>

namespace TinyFramework
{
	namespace IO
	{
		TinyBitReader::TinyBitReader()
			:m_bits(NULL),
			m_size(0),
			m_remainingBits(0),
			m_currentByte(0)
		{

		}

		TinyBitReader::~TinyBitReader()
		{

		}
		BOOL TinyBitReader::Initialize(const BYTE* bits, LONG size)
		{
			if (!bits || size <= 0)
				return FALSE;
			m_bits = bits;
			m_size = size;
			m_remainingBits = 0;
			m_currentByte = 0;
			if (m_size > 0)
			{
				m_currentByte = *m_bits;
				++m_bits;
				--m_size;
				m_remainingBits = 8;
			}
			return TRUE;
		}
		BOOL TinyBitReader::ReadBits(INT count, UINT64* s)
		{
			*s = 0;
			while (m_remainingBits != 0 && count != 0)
			{
				INT take = min(m_remainingBits, count);
				*s <<= take;
				*s += (m_currentByte >> (m_remainingBits - take));
				count -= take;
				m_remainingBits -= take;
				m_currentByte &= (1 << m_remainingBits) - 1;
				if (m_remainingBits == 0)
				{
					if (m_size != 0)
					{
						m_currentByte = *m_bits;
						++m_bits;
						--m_size;
						m_remainingBits = 8;
					}
				}
			}
			return (count == 0);
		}

		BOOL TinyBitReader::SkipBits(INT count)
		{
			while (count > 0 && count > m_remainingBits)
			{
				count -= m_remainingBits;
				m_remainingBits = 0;
				if (m_size != 0)
				{
					m_currentByte = *m_bits;
					++m_bits;
					--m_size;
					m_remainingBits = 8;
				}
				if (m_remainingBits == 0)
				{
					return (count == 0);
				}
			}
			UINT64 value;
			return ReadBits(count, &value);
		}

		UINT32 TinyBitReader::Available() const
		{
			return static_cast<UINT32>(8 * (m_size - 1) + m_remainingBits);
		}
	}
}
