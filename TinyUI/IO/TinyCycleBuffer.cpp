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
			m_wrap(SAME_WRAP),
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
			this->m_data.Reset(new CHAR[size]);
			if (!this->m_data)
				return FALSE;
			this->m_size = size;
			this->m_readPos = 0;
			this->m_writePos = 0;
			this->m_wrap = SAME_WRAP;
			memset(this->m_data, 0, this->m_size);
			return TRUE;
		}

		DWORD TinyCycleBuffer::Read(void* data, DWORD size)
		{
			TinyAutoLock lock(m_lock);
			if (!data)
				return 0;
			DWORD dwOUT = this->GetAvailableOUT();
			dwOUT = (dwOUT < size ? dwOUT : size);
			const DWORD dwOffset = this->m_size - this->m_readPos;
			if (dwOUT > dwOffset)
			{
				memcpy(data, this->m_data + this->m_readPos, dwOffset);
				memcpy(((CHAR*)data) + dwOffset, this->m_data, (dwOUT - dwOffset));
			}
			else
			{
				memcpy(data, this->m_data + this->m_readPos, dwOUT);
			}
			this->MoveReadPtr((LONG)dwOUT);
			return dwOUT;
		}

		DWORD TinyCycleBuffer::Write(const void* data, DWORD size)
		{
			TinyAutoLock lock(m_lock);
			if (!data)
				return 0;
			DWORD dwIN = this->GetAvailableIN();
			dwIN = (dwIN < size ? dwIN : size);
			DWORD dwT = dwIN;
			const DWORD dwOffset = this->m_size - this->m_writePos;
			if (dwIN > dwOffset)
			{
				memcpy(this->m_data + this->m_writePos, data, dwOffset);
				this->m_writePos = 0;
				dwT -= dwOffset;
				this->m_wrap = DIFF_WRAP;
			}
			memcpy(this->m_data + this->m_writePos, ((const CHAR*)data) + ((dwIN - dwT)), dwT);
			this->m_writePos += dwT;
			return dwIN;
		}

		DWORD TinyCycleBuffer::GetAvailable(BOOL bIN)
		{
			TinyAutoLock lock(m_lock);
			return bIN ? this->GetAvailableIN() : this->GetAvailableOUT();
		}

		LONG TinyCycleBuffer::MoveReadPtr(LONG size)
		{
			const LONG dwIN = (LONG)this->GetAvailableIN();
			const LONG dwOUT = (LONG)this->GetAvailableOUT();
			LONG pos = (LONG)this->m_readPos;
			if (size > dwOUT)
			{
				size = dwOUT;
			}
			if (size < -dwIN)
			{
				size = -dwIN;
			}
			pos += size;
			if (pos > (LONG) this->m_size)
			{
				pos -= (LONG)this->m_size;
				this->m_wrap = SAME_WRAP;
			}
			if (pos < 0)
			{
				pos += (LONG)this->m_size;
				this->m_wrap = DIFF_WRAP;
			}
			this->m_readPos = (size_t)pos;
			return size;
		}

		DWORD TinyCycleBuffer::GetAvailableOUT()
		{
			if (this->m_wrap == SAME_WRAP)
			{
				return this->m_writePos - this->m_readPos;
			}
			else
			{
				return this->m_size - this->m_readPos + this->m_writePos;
			}
		}

		DWORD TinyCycleBuffer::GetAvailableIN()
		{
			return this->m_size - this->GetAvailableOUT();
		}
	}
}
