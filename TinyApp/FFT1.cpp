#include "stdafx.h"
#include "FFT1.h"

namespace App
{
	MyFFT::MyFFT(int pSampleSize)
	{
		m_reals = NULL;
		m_imags = NULL;
		m_mags = NULL;
		m_sintable = NULL;
		m_costable = NULL;
		m_bitReverses = NULL;

		m_sampleSize = pSampleSize;
		m_sampleSize2 = m_sampleSize >> 1;
		m_iterations = (int)(log((float)m_sampleSize) / log((float)2));
		m_iterations1 = m_iterations - 1;

		m_reals = new float[m_sampleSize]; // real part
		m_imags = new float[m_sampleSize]; // image part
		m_mags = new float[m_sampleSize2];

		int sampleSize2 = m_sampleSize2;
		int iterations1 = m_iterations - 1;
		m_sintable = new float[m_iterations * sampleSize2];
		m_costable = new float[m_iterations * sampleSize2];
		int k = 0;
		int x = 0;
		for (int i = 1; i <= m_iterations; i++)
		{
			while (k < m_sampleSize)
			{
				for (int j = 1; j <= sampleSize2; j++)
				{
					float p = (float)ReverseBits(k >> iterations1, m_iterations);
					float arg = (6.283185F * p) / (float)m_sampleSize;
					m_sintable[x] = (float)sin(arg);
					m_costable[x] = (float)cos(arg);
					k++;
					x++;
				}

				k += sampleSize2;
			}

			k = 0;
			iterations1--;
			sampleSize2 >>= 1;
		}
		m_bitReverses = new int[m_sampleSize];
		for (k = 0; k < m_sampleSize; k++)
		{
			m_bitReverses[k] = ReverseBits(k, m_iterations);
		}
		//PrepareFFTTables();
	}

	MyFFT::~MyFFT(void)
	{
		if (m_reals != NULL)
			delete[] m_reals;

		if (m_imags != NULL)
			delete[] m_imags;

		if (m_mags != NULL)
			delete[] m_mags;

		if (m_sintable != NULL)
			delete[] m_sintable;

		if (m_costable != NULL)
			delete[] m_costable;

		if (m_bitReverses != NULL)
			delete[] m_bitReverses;

		m_reals = NULL;
		m_imags = NULL;
		m_mags = NULL;
		m_sintable = NULL;
		m_costable = NULL;
		m_bitReverses = NULL;
	}

	void MyFFT::PrepareFFTTables()
	{
		int sampleSize2 = m_sampleSize2;
		int iterations1 = m_iterations - 1;
		m_sintable = new float[m_iterations * sampleSize2];
		m_costable = new float[m_iterations * sampleSize2];
		int k = 0;
		int x = 0;
		for (int l = 1; l <= m_iterations; l++)
		{
			while (k < m_sampleSize)
			{
				for (int i = 1; i <= sampleSize2; i++)
				{
					float p = (float)ReverseBits(k >> iterations1, m_iterations);
					float arg = (6.283185F * p) / (float)m_sampleSize;
					m_sintable[x] = (float)sin(arg);
					m_costable[x] = (float)cos(arg);
					k++;
					x++;
				}

				k += sampleSize2;
			}

			k = 0;
			iterations1--;
			sampleSize2 >>= 1;
		}
		m_bitReverses = new int[m_sampleSize];
		for (k = 0; k < m_sampleSize; k++)
		{
			m_bitReverses[k] = ReverseBits(k, m_iterations);
		}
	}

	INT MyFFT::ReverseBits(INT index, INT NumBits)
	{
		INT i, rev;
		for (i = rev = 0; i < NumBits; i++)
		{
			rev = (rev << 1) | (index & 1);
			index >>= 1;
		}
		return rev;
	}

	float* MyFFT::Calculate(float* pSample, size_t sampleSize)
	{
		int ss2 = m_sampleSize2;
		int iterations1 = m_iterations - 1;
		int wAps = sampleSize / m_sampleSize;
		size_t a = 0;

		for (size_t b = 0; a < sampleSize; b++) {
			m_reals[b] = pSample[a];
			m_imags[b] = 0.0F;
			a += wAps;
		}

		int x = 0;
		for (int l = 1; l <= m_iterations; l++) {
			for (int k = 0; k < m_sampleSize; k += ss2) {
				for (int i = 1; i <= ss2; i++) {
					float c = m_costable[x];
					float s = m_sintable[x];
					int kn2 = k + ss2;
					float tr = m_reals[kn2] * c + m_imags[kn2] * s;
					float ti = m_imags[kn2] * c - m_reals[kn2] * s;
					m_reals[kn2] = m_reals[k] - tr;
					m_imags[kn2] = m_imags[k] - ti;
					m_reals[k] += tr;
					m_imags[k] += ti;
					k++;
					x++;
				}
			}

			iterations1--;
			ss2 >>= 1;
		}

		for (int k = 0; k < m_sampleSize; k++) {
			int r = m_bitReverses[k];
			if (r > k) {
				float tr = m_reals[k];
				float ti = m_imags[k];
				m_reals[k] = m_reals[r];
				m_imags[k] = m_imags[r];
				m_reals[r] = tr;
				m_imags[r] = ti;
			}
		}

		m_mags[0] = (float)sqrt(m_reals[0] * m_reals[0] + m_imags[0] * m_imags[0]) / (float)m_sampleSize;
		for (int i = 1; i < m_sampleSize2; i++)
			m_mags[i] = (2.0F * (float)sqrt(m_reals[i] * m_reals[i] + m_imags[i] * m_imags[i])) / (float)m_sampleSize;

		return m_mags;
	}

}