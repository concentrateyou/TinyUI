#include "../stdafx.h"
#include <process.h>
#include "TinyCycleBuffer.h"
#include <atomic>

namespace TinyUI
{
	namespace IO
	{
		TinyCycleBuffer::TinyCycleBuffer(TinyLock& lock, DWORD dwSize)
			:m_lock(lock),
			m_dwSize(dwSize)
		{

		}
		TinyCycleBuffer::~TinyCycleBuffer()
		{

		}
	}
}
