#pragma once
#include "Common.h"
#include <math.h>

namespace Decode
{
#define FFT_BUFFER_SIZE		512
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
		void Calculate();
	private:
		void Prepare();
	private:
		UINT		m_bitReverses[FFT_BUFFER_SIZE];
		FLOAT		m_reals[FFT_BUFFER_SIZE];
		FLOAT		m_imags[FFT_BUFFER_SIZE];
		FLOAT		m_sintable[FFT_BUFFER_SIZE / 2];
		FLOAT		m_costable[FFT_BUFFER_SIZE / 2];
	};
}


