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
		class TinyBitWriter
		{
		public:
			TinyBitWriter(BYTE *bits, INT size);
			~TinyBitWriter();
			BOOL WriteBits(INT count, UINT64 value);
			BOOL SkipBits(INT count);
			UINT32 Available() const;
		private:
			BYTE*	m_bits;
			INT		m_size;
			INT		m_consumeBits;
			INT		m_remainingBits;
		};
	};
}

