#pragma once
#include "Common.h"
#include <math.h>

namespace Decode
{
#define FFT_SAMPLE_SIZE		512
#define PI					3.14159265
	/// <summary>
	/// ���ٸ���Ҷ�任
	/// http://blog.jobbole.com/58246/
	/// </summary>
	class FFT
	{
		DISALLOW_COPY_AND_ASSIGN(FFT)
	public:
		FFT();
		~FFT();
		BOOL	Initialize(INT sampleSize = FFT_SAMPLE_SIZE);
		FLOAT*	Calculate(FLOAT* pSample, INT sampleSize);
	private:
		INT ReverseBits(INT val, INT iterations);
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


