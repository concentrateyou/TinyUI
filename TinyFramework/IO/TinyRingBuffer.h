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
		/// �̶�Ԫ�ش�Сѭ������
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
			DWORD	GetAvailableOUT();//�ɶ�ȡ������
			DWORD	GetAvailableIN();//��д�������
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

