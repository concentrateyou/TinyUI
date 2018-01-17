#include "../stdafx.h"
#include "TinyBitWriter.h"
#include <algorithm>

namespace TinyUI
{
	namespace IO
	{
		INT CountBits(UINT64 val)
		{
			INT count = 0;
			while (val != 0)
			{
				count++;
				val >>= 1;
			}
			return count;
		}


		TinyBitWriter::TinyBitWriter(BYTE *bits, INT size)
			:m_bits(bits),
			m_size(size),
			m_remainingBits(0),
			m_offsetBits(0),
			m_currentByte(0)
		{
			ZeroMemory(m_bits, m_size);
			if (m_size > 0)
			{
				m_remainingBits = 8;
			}
		}

		TinyBitWriter::~TinyBitWriter()
		{

		}

		BOOL TinyBitWriter::WriteBits(INT count, UINT64 value)
		{
			if (m_size <= 0 || value >= (1U << count))
				return FALSE;
			while (m_remainingBits != 0 && count != 0)
			{
				BYTE task = min(m_remainingBits, count);
				INT i = (m_remainingBits - (count > 8 ? 8 : count));
				BYTE s = 0;
				if (i >= 0)
				{
					s = ((BYTE)value & 0xFF);
					s <<= i;
				}
				else
				{
					s = ((BYTE)value & 0xFF);
					s >>= abs(i);
				}
				*m_bits += s;
				m_remainingBits -= task;
				count -= task;
				if (m_remainingBits == 0)
				{
					if (count > 0 && i >= 0)
					{
						*m_bits >>= count;
					}
					if (m_size != 0)
					{
						++m_bits;
						--m_size;
						m_remainingBits = 8;
					}
				}
			}
			return TRUE;

		}

		BOOL TinyBitWriter::SkipBits(INT count)
		{
			while (count > 0 && count > m_remainingBits)
			{
				count -= m_remainingBits;
				m_remainingBits = 0;
				if (m_size != 0)
				{
					++m_bits;
					--m_size;
					m_remainingBits = 8;
				}
				if (m_remainingBits == 0)
				{
					return (count == 0);
				}
			}
			UINT32 value = 0;
			return WriteBits(count, value);
		}

		UINT32 TinyBitWriter::Available() const
		{
			return static_cast<UINT32>(8 * (m_size - 1) + m_remainingBits);
		}
	}
}
