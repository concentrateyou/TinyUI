#include "../stdafx.h"
#include <process.h>
#include "TinyRingBuffer.h"
#include <atomic>

namespace TinyUI
{
	namespace IO
	{
		TinyRingBuffer::TinyRingBuffer()
			:m_count(0),
			m_size(0),
			m_readPos(0),
			m_writePos(0),
			m_wrap(SAME_WRAP)
		{

		}
		TinyRingBuffer::~TinyRingBuffer()
		{

		}

		DWORD TinyRingBuffer::GetElementSize() const
		{
			return m_size;
		}

		BOOL TinyRingBuffer::IsEmpty()
		{
			return m_bits != NULL;
		}

		BOOL TinyRingBuffer::Initialize(DWORD count, DWORD esize)
		{
			if (count == 0 || esize == 0)
				return FALSE;
			this->m_bits.Reset(new CHAR[count * esize]);
			if (!this->m_bits)
				return FALSE;
			this->m_count = count;
			this->m_size = esize;
			this->m_readPos = 0;
			this->m_writePos = 0;
			this->m_wrap = SAME_WRAP;
			memset(this->m_bits, 0, this->m_count * this->m_size);
			return TRUE;
		}

		DWORD TinyRingBuffer::Read(void* data, DWORD count)
		{
			if (!data)
				return 0;
			DWORD dwOUT = this->GetAvailableOUT();
			dwOUT = (dwOUT < count ? dwOUT : count);
			const DWORD dwOffset = this->m_count - this->m_readPos;
			if (dwOUT > dwOffset)
			{
				memcpy(data, this->m_bits + this->m_readPos * this->m_size, dwOffset * this->m_size);
				memcpy(((CHAR*)data) + dwOffset * this->m_size, this->m_bits, (dwOUT - dwOffset) * this->m_size);
			}
			else
			{
				memcpy(data, this->m_bits + this->m_readPos * this->m_size, dwOUT * this->m_size);
			}
			this->MoveReadPtr((LONG)dwOUT);
			return dwOUT;
		}

		DWORD TinyRingBuffer::Write(const void* data, DWORD count)
		{
			if (!data)
				return 0;
			DWORD dwIN = this->GetAvailableIN();
			dwIN = (dwIN < count ? dwIN : count);
			DWORD dwT = dwIN;
			const DWORD dwOffset = this->m_count - this->m_writePos;
			if (dwIN > dwOffset)
			{
				memcpy(this->m_bits + this->m_writePos * this->m_size, data, dwOffset * this->m_size);
				this->m_writePos = 0;
				dwT -= dwOffset;
				this->m_wrap = DIFF_WRAP;
			}
			memcpy(this->m_bits + this->m_writePos * this->m_size, ((const char*)data) + ((dwIN - dwT) * this->m_size), dwT * this->m_size);
			this->m_writePos += dwT;
			return dwIN;
		}

		LONG TinyRingBuffer::MoveReadPtr(LONG count)
		{
			const LONG dwIN = (LONG)this->GetAvailableIN();
			const LONG dwOUT = (LONG)this->GetAvailableOUT();
			LONG pos = (LONG)this->m_readPos;
			if (count > dwOUT)
			{
				count = dwOUT;
			}
			if (count < -dwIN)
			{
				count = -dwIN;
			}
			pos += count;
			if (pos > (LONG) this->m_count)
			{
				pos -= (LONG)this->m_count;
				this->m_wrap = SAME_WRAP;
			}
			if (pos < 0)
			{
				pos += (LONG)this->m_count;
				this->m_wrap = DIFF_WRAP;
			}
			this->m_readPos = (size_t)pos;
			return count;
		}

		DWORD TinyRingBuffer::GetAvailableOUT()
		{
			if (this->m_wrap == SAME_WRAP)
			{
				return this->m_writePos - this->m_readPos;
			}
			else
			{
				return this->m_count - this->m_readPos + this->m_writePos;
			}
		}

		DWORD TinyRingBuffer::GetAvailableIN()
		{
			return this->m_count - this->GetAvailableOUT();
		}
	}
}
