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
		/// Ñ­»·»º³å
		/// </summary>
		class TinyCircleBuffer
		{
		public:
			TinyCircleBuffer();
			~TinyCircleBuffer();
			BOOL	Allocate(DWORD count, DWORD size);
			void	DeAllocate();
			DWORD	Read(void** data_ptr, void* data, DWORD count);
			DWORD	Write(const void* data, DWORD count);
		protected:
			DWORD m_readPos;
			DWORD m_writePos;
			DWORD m_count;
			DWORD m_size;
			TinyScopedArray<CHAR> m_data;
		};
	};
}

