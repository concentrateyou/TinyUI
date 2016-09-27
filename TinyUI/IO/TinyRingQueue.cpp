#include "../stdafx.h"
#include <process.h>
#include "TinyRingQueue.h"

namespace TinyUI
{
	namespace IO
	{
		TinyRingQueue::TinyRingQueue()
		{
			ZeroMemory(&m_io, sizeof(m_io));
		}
		TinyRingQueue::~TinyRingQueue()
		{

		}
		BOOL TinyRingQueue::Initialize(UINT size)
		{
			if (IS_POWER_OF_2(size))
			{
				m_data.Reset(new BYTE[size]);
				m_io.data = m_data;
				m_io.size = size;
				m_io.in = 0;
				m_io.out = 0;
				return TRUE;
			}
			return FALSE;
		}
		UINT TinyRingQueue::GetSize()
		{
			TinyAutoLock lock(*this);
			return (m_io.in - m_io.out);
		}
		UINT TinyRingQueue::Read(BYTE *data, UINT size)
		{
			TinyAutoLock lock(*this);
			size = min(size, m_io.in - m_io.out);
			UINT s = min(size, m_io.size - (m_io.out & (m_io.size - 1)));
			memcpy(data, m_io.data + (m_io.out & (m_io.size - 1)), s);
			memcpy(data + s, m_io.data, size - s);
			m_io.out += size;
			if (m_io.in == m_io.out)
				m_io.in = m_io.out = 0;
			return s;
		}
		UINT TinyRingQueue::Write(BYTE *data, UINT size)
		{
			TinyAutoLock lock(*this);
			size = min(size, m_io.size - m_io.in + m_io.out);
			UINT s = min(size, m_io.size - (m_io.in & (m_io.size - 1)));
			memcpy(m_io.data + (m_io.in & (m_io.size - 1)), data, s);
			memcpy(m_io.data, data + s, size - s);
			m_io.in += size;
			return s;
		}
	}
}
