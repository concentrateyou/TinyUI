#pragma once
#include "Common.h"
#include <math.h>

namespace Decode
{
#define FFT_SAMPLE_SIZE		512
#define PI					3.14159265
	/// <summary>
	/// 快速傅里叶变换
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


