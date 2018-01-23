#pragma once
#include "Common.h"
#include <vector>
using namespace std;
using namespace TinyUI;

namespace Decode
{
	class H264BitReader
	{
		DISALLOW_COPY_AND_ASSIGN(H264BitReader)
	public:
		H264BitReader();
		~H264BitReader();
		BOOL Initialize(const BYTE* data, off_t size);
		template<typename T>
		BOOL ReadBits(INT count, T* s)
		{
			INT64 val;
			BOOL bRes = ReadBits(count, &val);
			*s = static_cast<T>(val);
			return bRes;
		}
		BOOL ReadBits(INT count, INT64* val);
		off_t Available();
		BOOL HasRBSP();
		size_t EmulationPreventionBytes();
	private:
		BOOL UpdateCurrentByte();
	private:
		const BYTE* m_bits;
		off_t		m_remainingBytes;
		BYTE		m_currentByte;
		INT			m_remainingBits;
		INT			m_prev2Bytes;
		size_t		m_emulationPreventionBytes;
	};
}

