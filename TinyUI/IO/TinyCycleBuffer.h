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
			TinyCycleBuffer(TinyLock& lock,DWORD dwSize);
			~TinyCycleBuffer();
		private:
			TinyLock&	m_lock;
			DWORD		m_dwSize;//ª∫¥Ê¥Û–°
		};
	};
}

