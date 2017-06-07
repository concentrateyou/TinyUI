#pragma once
#include "../Common/TinyCollection.h"
#include "../Common/TinyCore.h"
#include "../Common/TinyCallback.h"
#include "../Common/TinyEvent.h"
#include <string>
#include <algorithm>
using namespace std;

namespace TinyUI
{
	namespace IO
	{
		class TinyRingBuffer
		{
			enum Wrap { SAME_WRAP, DIFF_WRAP };
			DISALLOW_COPY_AND_ASSIGN(TinyRingBuffer)
		public:
			TinyRingBuffer(TinyLock& lock);
			~TinyRingBuffer();
			BOOL Initialize(DWORD count, DWORD size);
			DWORD Read(void* data, DWORD count);
			DWORD Write(const void* data, DWORD count);
		private:
			DWORD GetAvailableOUT();
			DWORD GetAvailableIN();
			LONG MoveReadPtr(LONG count);
		public:
			DWORD		m_readPos;
			DWORD		m_writePos;
			DWORD		m_count;
			DWORD		m_size;
			Wrap		m_wrap;
			TinyLock&	m_lock;
			TinyScopedArray<CHAR> m_data;
		};
	};
}

