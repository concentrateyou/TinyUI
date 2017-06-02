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
		class TinyCycleBuffer
		{
			enum Wrap { SAME_WRAP, DIFF_WRAP };
			DISALLOW_COPY_AND_ASSIGN(TinyCycleBuffer)
		public:
			TinyCycleBuffer(TinyLock& lock);
			~TinyCycleBuffer();
			BOOL Initialize(DWORD size);
			DWORD Read(void* data, DWORD size);
			DWORD Write(const void* data, DWORD size);
			DWORD GetAvailable(BOOL bIN);
		private:
			LONG MoveReadPtr(LONG size);
			DWORD GetAvailableOUT();
			DWORD GetAvailableIN();
		public:
			DWORD		m_readPos;
			DWORD		m_writePos;
			DWORD		m_size;
			Wrap		m_wrap;
			TinyLock&	m_lock;
			TinyScopedArray<CHAR> m_data;
		};

	};
}

