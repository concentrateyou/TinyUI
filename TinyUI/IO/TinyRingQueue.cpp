#include "../stdafx.h"
#include <process.h>
#include "TinyRingQueue.h"

namespace TinyUI
{
	namespace IO
	{
		TinyRingQueue::TinyRingQueue()
		{
			m_lock.Initialize();
		}
		TinyRingQueue::~TinyRingQueue()
		{
			m_lock.Uninitialize();
		}
		BOOL TinyRingQueue::Initialize(BYTE* buffer, UINT size)
		{
			if (is_power_of_2(size))
			{
				m_s.buffer = buffer;
				m_s.size = size;
				m_s.in = 0;
				m_s.out = 0;
				return TRUE;
			}
			return FALSE;
		}
		UINT TinyRingQueue::GetSize()
		{
			m_lock.Lock();
			UINT ts = (m_s.in - m_s.out);
			m_lock.Unlock();
			return ts;
		}
		UINT TinyRingQueue::InternalGet(BYTE *buffer, UINT size)
		{
			ASSERT(buffer);
			UINT ts = 0;
			size = min(size, m_s.in - m_s.out);
			ts = min(size, m_s.size - (m_s.out & (m_s.size - 1)));
			memcpy(buffer, m_s.buffer + (m_s.out & (m_s.size - 1)), ts);
			memcpy(buffer + ts, m_s.buffer, size - ts);
			m_s.out += size;
			return size;
		}
		UINT TinyRingQueue::InternalPut(BYTE *buffer, UINT size)
		{
			ASSERT(buffer);
			UINT ts = 0;
			size = min(size, m_s.size - m_s.in + m_s.out);
			ts = min(size, m_s.size - (m_s.in & (m_s.size - 1)));
			memcpy(m_s.buffer + (m_s.in & (m_s.size - 1)), buffer, ts);
			memcpy(m_s.buffer, buffer + ts, size - ts);
			m_s.in += size;
			return size;
		}
		UINT TinyRingQueue::Get(BYTE *buffer, UINT size)
		{
			UINT ts;
			m_lock.Lock();
			ts = InternalGet(buffer, size);
			if (m_s.in == m_s.out)
				m_s.in = m_s.out = 0;
			m_lock.Unlock();
			return ts;
		}
		UINT TinyRingQueue::Put(BYTE *buffer, UINT size)
		{
			UINT ts;
			m_lock.Lock();
			ts = InternalPut(buffer, size);
			m_lock.Unlock();
			return ts;
		}
	}
}
