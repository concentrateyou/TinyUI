#include "stdafx.h"
#include "ByteQueue.h"

namespace MediaSDK
{
	ByteQueue::ByteQueue()
		: m_buffer(new CHAR[1024]),
		m_size(1024),
		m_offset(0),
		m_count(0)
	{

	}

	ByteQueue::~ByteQueue() = default;

	void ByteQueue::Reset()
	{
		m_offset = 0;
		m_count = 0;
	}

	void ByteQueue::Push(const CHAR* data, INT32 size)
	{
		size_t total = m_count + size;
		if (total > m_size)
		{
			size_t newsize = 2 * m_size;
			while (total > newsize && newsize > m_size)
				newsize *= 2;
			std::unique_ptr<CHAR[]> new_buffer(new CHAR[newsize]);
			if (m_count > 0)
				memcpy(new_buffer.get(), front(), m_count);
			m_buffer = std::move(new_buffer);
			m_size = newsize;
			m_offset = 0;
		}
		else if ((m_offset + m_count + size) > m_size)
		{
			memmove(m_buffer.get(), front(), m_count);
			m_offset = 0;
		}
		memcpy(front() + m_count, data, size);
		m_count += size;
	}

	void ByteQueue::Peek(const CHAR** data, INT32* size) const
	{
		*data = front();
		*size = m_count;
	}

	void ByteQueue::Pop(INT32 count)
	{
		m_offset += count;
		m_count -= count;
		if (m_offset == m_size)
		{
			m_offset = 0;
		}
	}

	CHAR* ByteQueue::front() const
	{
		return m_buffer.get() + m_offset;
	}
}

