#pragma once
#include "../Common/TinyCollection.h"
#include "../Common/TinyCore.h"
#include "../Common/TinyCallback.h"
#include "../Common/TinyEvent.h"
#include <string>
#include <algorithm>
using namespace std;

namespace TinyFramework
{
	namespace IO
	{
		/// <summary>
		/// 固定元素大小循环缓冲
		/// </summary>
		class TinyRingBuffer
		{
			enum Wrap { SAME_WRAP, DIFF_WRAP };
			DISALLOW_COPY_AND_ASSIGN(TinyRingBuffer)
		public:
			TinyRingBuffer();
			virtual ~TinyRingBuffer();
			DWORD	GetElementSize() const;
			BOOL	IsEmpty();
			BOOL	Initialize(DWORD count, DWORD esize);
			DWORD	Read(void* data, DWORD count);
			DWORD	Write(const void* data, DWORD count);
			DWORD	GetAvailableOUT();//可读取的数据
			DWORD	GetAvailableIN();//可写入的数据
		private:
			LONG	MoveReadPtr(LONG count);
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

