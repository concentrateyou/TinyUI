#pragma once
#include "../Common/TinyCollection.h"
#include "../Common/TinyCore.h"
#include "../Common/TinyCallback.h"
#include "../Common/TinyEvent.h"
#include <string>
#include <algorithm>
using namespace std;

#ifndef IS_POWER_OF_2
#define IS_POWER_OF_2(x)   ((x) != 0 && (((x) & ((x) - 1)) == 0))
#endif 
namespace TinyUI
{
	namespace IO
	{
		/// <summary>
		/// 高效的循环队列(Linux FIFO)
		/// 大小必须是2的整数幂
		/// </summary>
		class TinyRingQueue : public TinyLock
		{
			typedef struct tagFIFO
			{
				BYTE*	data;
				UINT	size;
				UINT	offsetI;
				UINT	offsetO;
			}FIFO;
		public:
			TinyRingQueue();
			~TinyRingQueue();
			BOOL	Initialize(UINT size);
			UINT	GetSize();
			UINT	Read(BYTE *data, UINT size);
			UINT	Write(BYTE *data, UINT size);
			void	Reset();
		protected:
			TinyScopedArray<BYTE>	m_data;
			FIFO	m_io;
		};
	};
}

