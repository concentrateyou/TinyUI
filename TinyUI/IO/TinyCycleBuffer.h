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
			DISALLOW_COPY_AND_ASSIGN(TinyCycleBuffer)
		public:
			TinyCycleBuffer(TinyLock& lock);
			virtual ~TinyCycleBuffer();
			BOOL Initialize(DWORD size);
			DWORD Read(void* data, DWORD count);
			DWORD Write(const void* data, DWORD count);
		public:
			DWORD		m_readPos;
			DWORD		m_writePos;
			DWORD		m_size;
			TinyLock&	m_lock;
			TinyScopedArray<CHAR> m_data;
		};

	};
}

