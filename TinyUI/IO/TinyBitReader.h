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
			TinyBitReader(const BYTE* bits, LONG size);
			~TinyBitReader();
			template<typename T>
			BOOL ReadBits(INT count, T* out)
			{
				UINT64 val;
				BOOL bRes = ReadBitsInternal(count, &val);
				*out = static_cast<T>(val);
				return bRes;
			}
			BOOL SkipBits(INT count);
			UINT32 Available() const;
		private:
			BOOL ReadBitsInternal(INT count, UINT64* out);
		private:
			const BYTE*		m_bits;
			LONG			m_size;
			BYTE			m_currentByte;
			INT				m_remainingBits;
		};
	};
}

