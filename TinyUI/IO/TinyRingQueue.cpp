#include "../stdafx.h"
#include <process.h>
#include "TinyRingQueue.h"
#include <atomic>

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
			if (size == m_io.size)
				return TRUE;
			if (IS_POWER_OF_2(size))
			{
				m_data.Reset(new CHAR[size]);
				memset(m_data, 0, size * sizeof(CHAR));
				m_io.data = m_data;
				m_io.size = size;
				m_io.offsetI = 0;
				m_io.offsetO = 0;
				return TRUE;
			}
			return FALSE;
		}
		UINT TinyRingQueue::GetAvailable(BOOL bIN)
		{
			return bIN ? (m_io.size - (m_io.offsetI - m_io.offsetO)) : (m_io.offsetI - m_io.offsetO);
		}
		UINT TinyRingQueue::Read(CHAR *data, UINT size)
		{
			BOOL bRes = ReadBytes(data, size);
			if (m_io.offsetI == m_io.offsetO)
			{
				m_io.offsetI = m_io.offsetO = 0;
			}
			return bRes;
		}
		UINT TinyRingQueue::Write(CHAR *data, UINT size)
		{
			BOOL bRes = WriteBytes(data, size);
			return bRes;
		}
		UINT TinyRingQueue::ReadBytes(CHAR *data, UINT size)
		{
			size = min(size, m_io.offsetI - m_io.offsetO);
			UINT s = min(size, m_io.size - (m_io.offsetO & (m_io.size - 1)));
			memcpy(data, m_io.data + (m_io.offsetO & (m_io.size - 1)), s);
			memcpy(data + s, m_io.data, size - s);
			m_io.offsetO += size;
			return size;
		}
		UINT TinyRingQueue::WriteBytes(CHAR *data, UINT size)
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
			m_io.offsetI = m_io.offsetO = 0;
		}
	}
}
