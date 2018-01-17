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
#define AV_WB32(p, val) do {                 \
        uint32_t d = (val);                     \
        ((uint8_t*)(p))[3] = (d);               \
        ((uint8_t*)(p))[2] = (d)>>8;            \
        ((uint8_t*)(p))[1] = (d)>>16;           \
        ((uint8_t*)(p))[0] = (d)>>24;           \
    } while(0)

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

