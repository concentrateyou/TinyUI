#include "../stdafx.h"
#include "TinyCircleBuffer.h"
#include <atomic>

namespace TinyUI
{
	namespace IO
	{
		TinyCircleBuffer::TinyCircleBuffer()
			:m_readPos(0),
			m_writePos(0),
			m_count(0),
			m_size(0)
		{

		}
		TinyCircleBuffer::~TinyCircleBuffer()
		{

		}
		BOOL TinyCircleBuffer::Allocate(DWORD count, DWORD size)
		{
			m_data.Reset(new CHAR[count * size]);
			if (!m_data)
				return FALSE;
			memset(m_data, 0, count * size);
			m_count = count;
			m_size = size;
		}
		void TinyCircleBuffer::DeAllocate()
		{
			m_data.Reset(NULL);
		}
		DWORD TinyCircleBuffer::Read(void** data_ptr, void* data, DWORD count)
		{
			return 0;
		}
		DWORD TinyCircleBuffer::Write(const void* data, DWORD count)
		{
			return 0;
		}
	}
}
