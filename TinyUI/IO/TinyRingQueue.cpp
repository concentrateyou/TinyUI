#include "../stdafx.h"
#include <process.h>
#include "TinyRingQueue.h"
#include <atomic>

namespace TinyUI
{
	namespace IO
	{
		TinyRingQueue::TinyRingQueue(TinyLock& lock)
			:m_lock(lock)
		{
			ZeroMemory(&m_io, sizeof(m_io));
		}
		TinyRingQueue::~TinyRingQueue()
		{

		}
		BOOL TinyRingQueue::Initialize(UINT size)
		{
			if (size == m_io.size)
				return TRUE;
			if (IS_POWER_OF_2(size))
			{
				m_data.Reset(new BYTE[size]);
				memset(m_data, 0, size * sizeof(BYTE));
				m_io.data = m_data;
				m_io.size = size;
				m_io.offsetI = 0;
				m_io.offsetO = 0;
				return TRUE;
			}
			return FALSE;
		}
		UINT TinyRingQueue::GetSize()
		{
			return (m_io.offsetI - m_io.offsetO);
		}
		UINT TinyRingQueue::Read(BYTE *data, UINT size)
		{
			m_lock.Lock();
			BOOL bRes = ReadBytes(data, size);
			if (m_io.offsetI == m_io.offsetO)
			{
				m_io.offsetI = m_io.offsetO = 0;
			}
			m_lock.Unlock();
			return bRes;
		}
		UINT TinyRingQueue::Write(BYTE *data, UINT size)
		{
			m_lock.Lock();
			BOOL bRes = WriteBytes(data, size);
			m_lock.Unlock();
			return bRes;
		}
		UINT TinyRingQueue::ReadBytes(BYTE *data, UINT size)
		{
			size = min(size, m_io.offsetI - m_io.offsetO);
			UINT s = min(size, m_io.size - (m_io.offsetO & (m_io.size - 1)));
			memcpy(data, m_io.data + (m_io.offsetO & (m_io.size - 1)), s);
			memcpy(data + s, m_io.data, size - s);
			m_io.offsetO += size;
			return size;
		}
		UINT TinyRingQueue::WriteBytes(BYTE *data, UINT size)
		{
			size = min(size, m_io.size - m_io.offsetI + m_io.offsetO);
			UINT s = min(size, m_io.size - (m_io.offsetI & (m_io.size - 1)));
			memcpy(m_io.data + (m_io.offsetI & (m_io.size - 1)), data, s);
			memcpy(m_io.data, data + s, size - s);
			m_io.offsetI += size;
			return size;
		}
		void TinyRingQueue::Reset()
		{
			m_lock.Lock();
			m_io.offsetI = m_io.offsetO = 0;
			m_lock.Unlock();
		}
	}
}
