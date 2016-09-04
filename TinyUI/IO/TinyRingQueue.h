#pragma once
#include "../Common/TinyCollection.h"
#include "../Common/TinyCore.h"
#include "../Common/TinyCallback.h"
#include "../Common/TinyEvent.h"
#include <string>
#include <algorithm>
using namespace std;

#define is_power_of_2(x)   ((x) != 0 && (((x) & ((x) - 1)) == 0))

namespace TinyUI
{
	namespace IO
	{
		/// <summary>
		/// 高效的循环队列(Linux kfifo)
		/// </summary>
		class TinyRingQueue
		{
			typedef struct tagFIFO
			{
				BYTE*	buffer;
				UINT	size;
				UINT	in;
				UINT	out;
			}FIFO;
		public:
			TinyRingQueue();
			~TinyRingQueue();
			BOOL	Initialize(BYTE* buffer, UINT size);
			UINT	GetSize();
			UINT	Get(BYTE *buffer, UINT size);
			UINT	Put(BYTE *buffer, UINT size);
		private:
			UINT	InternalGet(BYTE *buffer, UINT size);
			UINT	InternalPut(BYTE *buffer, UINT size);
		private:
			FIFO			m_s;
			TinySpinLock	m_lock;
		};
	};
}

