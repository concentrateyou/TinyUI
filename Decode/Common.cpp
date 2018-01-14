#include "stdafx.h"
#include "Common.h"

namespace Decode
{
	INT ToINT32(BYTE val[4])
	{
		return (INT)(((val[0] & 0xFF) << 24) |
			((val[1] & 0xFF) << 16) |
			((val[2] & 0xFF) << 8) |
			((val[3] & 0xFF)));
	}
	INT ToINT24(BYTE val[3])
	{
		return (INT)(((val[0] & 0xFF) << 16) |
			((val[1] & 0xFF) << 8) |
			((val[2] & 0xFF)));
	}
	INT ToINT16(BYTE val[2])
	{
		return	(INT)(val[0] << 8) | (val[1]);
	}
	INT ToINT8(BYTE val[1])
	{
		return	(INT)(val[0] << 8);
	}
	BOOL FindStartCode(BYTE* bits, INT size)
	{
		for (INT i = 0;i < size - 1;i++)
		{
			if (bits[i] != 0)
				return FALSE;
		}
		if (bits[size - 1] != 1)
			return FALSE;
		return TRUE;
	}
	//////////////////////////////////////////////////////////////////////////
	BOOL TS_PACKET_PROGRAM::operator == (const TS_PACKET_PROGRAM& other)
	{
		return ProgramPID == other.ProgramPID;
	}
	//////////////////////////////////////////////////////////////////////////
	ByteQueue::ByteQueue()
		: m_io(new BYTE[1024]),
		m_size(1024),
		m_offset(0),
		m_remaining(0)
	{
	}

	ByteQueue::~ByteQueue()
	{
	}

	void ByteQueue::Reset()
	{
		m_offset = 0;
		m_remaining = 0;
	}

	void ByteQueue::Push(const BYTE* data, INT size)
	{
		size_t value = m_remaining + size;
		if (value > m_size)
		{
			size_t newsize = 2 * m_size;
			while (value > newsize && newsize > m_size)
				newsize *= 2;
			std::unique_ptr<BYTE[]> data(new BYTE[newsize]);
			if (m_remaining > 0)
				memcpy(data.get(), m_io.get() + m_offset, m_remaining);
			m_io.reset(data.release());
			m_size = newsize;
			m_offset = 0;
		}
		else if ((m_offset + m_remaining + size) > m_size)
		{
			memmove(m_io.get(), m_io.get() + m_offset, m_remaining);
			m_offset = 0;
		}
		memcpy(m_io.get() + m_offset + m_remaining, data, size);
		m_remaining += size;
	}

	void ByteQueue::Peek(const BYTE** data, INT* size) const
	{
		*data = (m_io.get() + m_offset);
		*size = m_remaining;
	}

	void ByteQueue::Pop(INT count)
	{
		m_offset += count;
		m_remaining -= count;
		if (m_offset == m_size)
		{
			m_offset = 0;
		}
	}
}