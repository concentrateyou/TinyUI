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
		/// ��Ч��ѭ������(Linux FIFO)
		/// ��С������2��������
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

