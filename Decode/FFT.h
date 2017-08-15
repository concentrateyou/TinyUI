#pragma once
#include "Common.h"
#include <math.h>

namespace Decode
{
	class FFT
	{
		DISALLOW_COPY_AND_ASSIGN(FFT)
	public:
		FFT();
		~FFT();
		BOOL	Initialize(INT sampleSize);
		FLOAT*	Calculate(FLOAT* pSample, INT sampleSize);
	private:
		static INT	ReverseBits(INT index, INT size);
	private:
		INT						m_sampleSize;//��������
		INT						m_iterations;//��������
		TinyScopedArray<INT>	m_bitReverses;
		TinyScopedArray<FLOAT>	m_imags;//�鲿
		TinyScopedArray<FLOAT>	m_reals;//ʵ��
		TinyScopedArray<FLOAT>	m_mags;//�ź�ǿ��
		TinyScopedArray<FLOAT>	m_sintable;
		TinyScopedArray<FLOAT>	m_costable;
	};
}


