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
		INT						m_sampleSize;//采样个数
		INT						m_iterations;//迭代次数
		TinyScopedArray<INT>	m_bitReverses;
		TinyScopedArray<FLOAT>	m_imags;//虚部
		TinyScopedArray<FLOAT>	m_reals;//实部
		TinyScopedArray<FLOAT>	m_mags;//信号强度
		TinyScopedArray<FLOAT>	m_sintable;
		TinyScopedArray<FLOAT>	m_costable;
	};
}


