#include "../stdafx.h"
#include <process.h>
#include "TinyCycleBuffer.h"
#include <atomic>

namespace TinyUI
{
	namespace IO
	{
		TinyCycleBuffer::TinyCycleBuffer(TinyLock& lock)
			:m_size(0),
			m_readPos(0),
			m_writePos(0),
			m_lock(lock)
		{

		}

		TinyCycleBuffer::~TinyCycleBuffer()
		{

		}

		BOOL TinyCycleBuffer::Initialize(DWORD size)
		{
			if (size == 0)
				return FALSE;
			m_size = size;
			m_data.Reset(new CHAR[size]);
			if (!this->m_data)
				return FALSE;
			memset(this->m_data, 0, this->m_size);
			return TRUE;
		}

		DWORD TinyCycleBuffer::Read(void* data, DWORD size)
		{
			TinyAutoLock lock(m_lock);
			if (!data)
				return 0;
			m_readPos += size;

		}

		DWORD TinyCycleBuffer::Write(const void* data, DWORD size)
		{
			return 0;
		}
	}
}
