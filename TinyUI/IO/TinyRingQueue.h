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
		/// 大小必须是2的整数幂,
		/// </summary>
		class TinyRingQueue
		{
			typedef struct tagFIFO
			{
				BYTE*	data;
				UINT	size;
				UINT	offsetI;
				UINT	offsetO;
			}FIFO;
		public:
			TinyRingQueue(TinyLock& lock);
			~TinyRingQueue();
			BOOL	Initialize(UINT size);
			UINT	GetSize();
			void	Reset();
			UINT	Read(BYTE *data, UINT size);
			UINT	Write(BYTE *data, UINT size);
		private:
			UINT	ReadBytes(BYTE *data, UINT size);
			UINT	WriteBytes(BYTE *data, UINT size);
		protected:
			TinyLock&				m_lock;
			TinyScopedArray<BYTE>	m_data;
			FIFO					m_io;
		};
	};
}

