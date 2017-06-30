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
		/// <summary>
		/// ѭ������
		/// </summary>
		class TinyRingBuffer
		{
			enum Wrap { SAME_WRAP, DIFF_WRAP };
			DISALLOW_COPY_AND_ASSIGN(TinyRingBuffer)
		public:
			TinyRingBuffer();
			virtual ~TinyRingBuffer();
			BOOL	Initialize(DWORD count, DWORD esize);
			DWORD	Read(void* data, DWORD count);
			DWORD	Write(const void* data, DWORD count);
			/// <summary>
			/// ��д�������
			/// </summary>
			DWORD	GetAvailableOUT();
			/// <summary>
			/// ʣ������
			/// </summary>
			DWORD	GetAvailableIN();
		private:
			LONG MoveReadPtr(LONG count);
		public:
			DWORD		m_readPos;
			DWORD		m_writePos;
			DWORD		m_count;
			DWORD		m_size;
			Wrap		m_wrap;
			TinyScopedArray<CHAR> m_bits;
		};
	};
}

