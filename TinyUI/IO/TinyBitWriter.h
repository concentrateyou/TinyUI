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
		/// 位写
		/// </summary>
		class TinyBitWriter
		{
		public:
			TinyBitWriter();
			~TinyBitWriter();
			BOOL Initialize(BYTE* bits, LONG size);
			BOOL WriteBits(INT count, UINT64 value);
			BOOL SkipBits(INT count);
			UINT32 Available() const;
		private:
			BYTE*		m_bits;
			LONG		m_size;
			INT			m_consumeBits;
			INT			m_remainingBits;
		};
	};
}

