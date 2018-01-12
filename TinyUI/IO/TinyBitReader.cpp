#include "../stdafx.h"
#include "TinyBitReader.h"
#include <algorithm>

namespace TinyUI
{
	namespace IO
	{
		TinyBitReader::TinyBitReader(const BYTE* bits, LONG size)
			:m_bits(bits),
			m_size(size),
			m_remainingBits(0)
		{
			if (m_size != 0)
			{
				m_currentByte = *m_bits;
				++m_bits;
				--m_size;
				m_remainingBits = 8;
			}
		}

		TinyBitReader::~TinyBitReader()
		{

		}

		BOOL TinyBitReader::ReadBitsInternal(INT count, UINT64* out)
		{
			*out = 0;
			while (m_remainingBits != 0 && count != 0)
			{
				INT takeBit = min(m_remainingBits, count);
				*out <<= takeBit;
				*out += m_currentByte >> (m_remainingBits - takeBit);
				count -= takeBit;
				m_remainingBits -= takeBit;
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
			return ReadBitsInternal(count, &value);
		}

		UINT32 TinyBitReader::Available() const
		{
			return static_cast<UINT32>(8 * m_size + m_remainingBits);
		}
	}
}
