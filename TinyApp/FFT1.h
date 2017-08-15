#pragma once
#include "Common.h"
#include <math.h>

namespace App
{
	/// <summary>
	/// 快速傅里叶变换
	/// http://blog.jobbole.com/58246/
	/// </summary>
	//class FFT
	//{
	//	DISALLOW_COPY_AND_ASSIGN(FFT)
	//public:
	//	FFT();
	//	~FFT();
	//	BOOL	Initialize(INT sampleSize = FFT_SAMPLE_SIZE);
	//	FLOAT*	Calculate(FLOAT* pSample, INT sampleSize);
	//private:
	//	INT ReverseBits(INT val, INT iterations);
	//private:
	//	INT						m_sampleSize;//采样个数
	//	INT						m_iterations;//迭代次数
	//	TinyScopedArray<INT>	m_bitReverses;
	//	TinyScopedArray<FLOAT>	m_imags;//虚部
	//	TinyScopedArray<FLOAT>	m_reals;//实部
	//	TinyScopedArray<FLOAT>	m_mags;//信号强度
	//	TinyScopedArray<FLOAT>	m_sintable;
	//	TinyScopedArray<FLOAT>	m_costable;
	//};
	class MyFFT
	{
	private:
		float* m_reals;
		float* m_imags;
		float* m_mags;
		float* m_sintable;
		float* m_costable;
		int* m_bitReverses;
		int m_sampleSize;
		int m_sampleSize2;
		int m_iterations;
		int m_iterations1;

		int ReverseBits(int index, int NumBits);
		void PrepareFFTTables();
	public:
		MyFFT(int pSampleSize);
		~MyFFT(void);

		float* Calculate(float* pSample, size_t pSampleSize);
	};
}


