#pragma once
#include "Common.h"
#include <vector>
using namespace std;
using namespace TinyFramework;

namespace Decode
{
	class H264BitReader
	{
		DISALLOW_COPY_AND_ASSIGN(H264BitReader)
	public:
		H264BitReader();
		~H264BitReader();
		BOOL Initialize(const BYTE* data, off_t size);
		BOOL ReadBits(INT count, INT32* val);//ÿ�ζ�ȡС��32λ���ܴ���0x03���
		off_t Available();
		BOOL HasRBSP();
		size_t EmulationPreventionBytes();//��ö����ֽ���
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

