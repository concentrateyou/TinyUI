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
		class TinyBitReader
		{
		public:
			TinyBitReader(BYTE* bits, LONG size);
			~TinyBitReader();
			template<typename T>
			BOOL ReadBits(INT count, T* s)
			{
				UINT64 val;
				BOOL bRes = ReadBits(count, &val);
				*s = static_cast<T>(val);
				return bRes;
			}
			BOOL ReadBits(INT count, UINT64* s);
			BOOL SkipBits(INT count);
			UINT32 Available() const;
		private:
			BYTE*			m_bits;
			LONG			m_size;
			BYTE			m_currentByte;
			INT				m_remainingBits;
		};
	};
}

