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
			ZeroMemory(bits, size);
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
			if (m_size <= 0)
				return FALSE;
			BYTE save = 0;
			while (m_remainingBits != 0 && count != 0)
			{
				INT take = min(m_remainingBits, count);
				BYTE currentByte = *m_bits << (8 - m_remainingBits);
				currentByte |= save;
				currentByte |= (value << (sizeof(UINT64) - take));
				value >>= m_remainingBits;
				*m_bits |= (currentByte >> (8 - m_remainingBits));
				m_remainingBits -= take;
				count -= take;
				if (m_remainingBits == 0)
				{
					if (count > 0)
					{
						save |= *m_bits << (8 - count);
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
			return count == 0;
		}
	}
}
