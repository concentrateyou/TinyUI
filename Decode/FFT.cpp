#include "stdafx.h"
#include "FFT.h"

namespace Decode
{

	FFT::FFT()
	{


	}
	FFT::~FFT()
	{
	}
	BOOL FFT::Initialize(INT sampleSize)
	{
		m_sampleSize = sampleSize;
		INT sampleSize2 = m_sampleSize / 2;
		m_iterations = (DWORD)(log((FLOAT)m_sampleSize) / log((FLOAT)2));//log2N
		DWORD iterations1 = m_iterations - 1;
		m_bitReverses.Reset(new INT[m_sampleSize]);
		if (!m_bitReverses)
			return FALSE;
		m_imags.Reset(new FLOAT[m_sampleSize]);
		if (!m_imags)
			return FALSE;
		m_reals.Reset(new FLOAT[m_sampleSize]);
		if (!m_reals)
			return FALSE;
		m_mags.Reset(new FLOAT[sampleSize2]);
		if (!m_mags)
			return FALSE;
		m_sintable.Reset(new FLOAT[m_iterations * sampleSize2]);
		if (!m_sintable)
			return FALSE;
		m_costable.Reset(new FLOAT[m_iterations * sampleSize2]);
		if (!m_costable)
			return FALSE;
		INT k = 0;
		INT x = 0;
		for (INT a = 1; a <= m_iterations; a++)
		{
			while (k < m_sampleSize)
			{
				for (INT i = 1; i <= sampleSize2; i++)
				{
					FLOAT val = (FLOAT)ReverseBits(k >> iterations1, m_iterations);
					FLOAT arg = static_cast<FLOAT>((PI * 2 * val) / (FLOAT)m_sampleSize);
					m_sintable[x] = (FLOAT)sin(arg);
					m_costable[x] = (FLOAT)cos(arg);
					k++;
					x++;
				}
				k += sampleSize2;
			}
			k = 0;
			iterations1--;
			sampleSize2 >>= 1;
		}
		for (k = 0; k < m_sampleSize; k++)
		{
			m_bitReverses[k] = ReverseBits(k, m_iterations);
		}
		return TRUE;
	}

	INT FFT::ReverseBits(INT val, INT iterations)
	{
		INT val1 = val;
		INT k = 0;
		for (INT i = 1; i <= iterations; i++)
		{
			INT val2 = val1 >> 1;
			k = ((k << 1) + val1) - (val2 << 1);
			val1 = val2;
		}
		return k;
	}

	FLOAT* FFT::Calculate(FLOAT* pSample, INT sampleSize)
	{
		INT sampleSize2 = m_sampleSize >> 1;
		INT iterations1 = m_iterations - 1;
		INT wAps = sampleSize / m_sampleSize;
		INT a = 0;
		for (INT b = 0; a < sampleSize; b++)
		{
			m_reals[b] = pSample[a];
			m_imags[b] = 0.0F;
			a += wAps;
		}
		INT x = 0;
		for (INT l = 1; l <= m_iterations; l++)
		{
			for (INT k = 0; k < m_sampleSize; k += sampleSize2)
			{
				for (INT i = 1; i <= sampleSize2; i++)
				{
					FLOAT c = m_costable[x];
					FLOAT s = m_sintable[x];
					INT kn2 = k + sampleSize2;
					FLOAT tr = m_reals[kn2] * c + m_imags[kn2] * s;
					FLOAT ti = m_imags[kn2] * c - m_reals[kn2] * s;
					m_reals[kn2] = m_reals[k] - tr;
					m_imags[kn2] = m_imags[k] - ti;
					m_reals[k] += tr;
					m_imags[k] += ti;
					k++;
					x++;
				}
			}
			iterations1--;
			sampleSize2 >>= 1;
		}
		for (INT k = 0; k < m_sampleSize; k++)
		{
			INT bit = m_bitReverses[k];
			if (bit > k)
			{
				float tr = m_reals[k];
				float ti = m_imags[k];
				m_reals[k] = m_reals[bit];
				m_imags[k] = m_imags[bit];
				m_reals[bit] = tr;
				m_imags[bit] = ti;
			}
		}
		m_mags[0] = (FLOAT)sqrt(m_reals[0] * m_reals[0] + m_imags[0] * m_imags[0]) / (FLOAT)m_sampleSize;
		for (INT i = 1; i < sampleSize2; i++)
		{
			m_mags[i] = (2.0F * (FLOAT)sqrt(m_reals[i] * m_reals[i] + m_imags[i] * m_imags[i])) / (FLOAT)m_sampleSize;
		}
		return m_mags;
	}
}