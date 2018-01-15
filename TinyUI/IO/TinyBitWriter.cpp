#include "../stdafx.h"
#include "TinyBitWriter.h"
#include <algorithm>

namespace TinyUI
{
	namespace IO
	{
		TinyBitWriter::TinyBitWriter(BYTE *bits, INT size)
			:m_bits(bits),
			m_size(size),
			m_remainingBits(0)
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

		BOOL TinyBitWriter::WriteBits(INT count, UINT32 value)
		{
			if (m_size <= 0 || value >= (1U << count))
			{
				return FALSE;
			}
			BYTE s = 0;
			while (m_remainingBits != 0 && count != 0)
			{
				INT take = min(m_remainingBits, count);
				BYTE val = *m_bits << (8 - m_remainingBits);
				val |= s;
				val |= (value << (32 - take));
				value >>= m_remainingBits;
				*m_bits |= (val >> (8 - m_remainingBits));
				m_remainingBits -= take;
				count -= take;
				s = 0;
				if (m_remainingBits == 0)
				{
					if (count > 0)
					{
						s |= *m_bits << ((8 - count) <= 0 ? 0 : (8 - count));
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
			return (count == 0);
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
