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
		//////////////////////////////////////////////////////////////////////////
		TinyBitWriter::TinyBitWriter()
			:m_bits(NULL),
			m_size(0),
			m_remainingBits(0),
			m_consumeBits(0)
		{
		}

		TinyBitWriter::~TinyBitWriter()
		{

		}
		BOOL TinyBitWriter::Initialize(BYTE* bits, LONG size)
		{
			if (!bits || size <= 0)
				return FALSE;
			m_bits = bits;
			m_size = size;
			ZeroMemory(m_bits, m_size);
			m_consumeBits = 0;
			if (m_size > 0)
			{
				m_remainingBits = 8;
			}
			return TRUE;
		}
		BOOL TinyBitWriter::WriteBits(INT count, UINT64 value)
		{
			if (m_size <= 0 || value >= (1U << count))
				return FALSE;
			value <<= (sizeof(UINT64) * 8 - count);
			while (m_remainingBits != 0 && count != 0)
			{
				INT take = min(m_remainingBits, count);
				BYTE s = static_cast<BYTE>(value >> 56);//取高8位
				s >>= m_consumeBits;
				*m_bits |= s;
				m_remainingBits -= take;
				count -= take;
				if (m_remainingBits != 0)
				{
					m_consumeBits = 8 - m_remainingBits;
				}
				else
				{
					if (count > 0)
					{
						value <<= 8 - m_consumeBits;
					}
					if (m_size != 0)
					{
						++m_bits;
						--m_size;
						m_remainingBits = 8;
						m_consumeBits = 0;
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
			return WriteBits(count, 0U);
		}

		UINT32 TinyBitWriter::Available() const
		{
			return static_cast<UINT32>(8 * (m_size - 1) + m_remainingBits);
		}
	}
}
