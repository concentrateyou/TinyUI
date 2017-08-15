#include "stdafx.h"
#include "AudioAnalyser.h"

namespace Decode
{
	AudioAnalyser::AudioAnalyser()
		:m_sampleSize(0),
		m_multiplier(0)
	{
	}

	AudioAnalyser::~AudioAnalyser()
	{
	}

	BOOL AudioAnalyser::Initialize()
	{
		m_size.cx = 300;
		m_size.cy = 180;
		m_sampleSize = DEFAULT_SAMPLE_SIZE;
		m_decay = DEFAULT_SPECTRUM_ANALYSER_DECAY;
		m_count = DEFAULT_SPECTRUM_ANALYSER_BAND_COUNT;
		m_oldFFT.Reset(new FLOAT[DEFAULT_SPECTRUM_ANALYSER_FFT_SAMPLE_SIZE]);
		if (!m_oldFFT)
			return FALSE;
		memset(m_oldFFT, 0, DEFAULT_SPECTRUM_ANALYSER_FFT_SAMPLE_SIZE);
		m_samples.Reset(new FLOAT[m_sampleSize]);
		if (!m_samples)
			return FALSE;
		memset(m_samples, 0, m_sampleSize);
		if (!m_audioFFT.Initialize(DEFAULT_SPECTRUM_ANALYSER_FFT_SAMPLE_SIZE))
			return FALSE;
		m_multiplier = static_cast<INT>(((DEFAULT_SPECTRUM_ANALYSER_FFT_SAMPLE_SIZE / 2) / m_count));
		return TRUE;
	}

	BOOL AudioAnalyser::Process(HWND hWND, BYTE* bits, LONG size)
	{
		INT j = 0;
		for (INT i = 0; i < m_sampleSize;i++)
		{
			FLOAT valL = (FLOAT)((bits[j + 1] << 8) + bits[j]) / 32767.0F;
			FLOAT valR = (FLOAT)((bits[j + 3] << 8) + bits[j + 2]) / 32767.0F;
			m_samples[i] = (valL + valR) / 2.0F;
			j += 4;
		}
		FLOAT* wFFT = m_audioFFT.Calculate(m_samples, m_sampleSize);
		FLOAT wRatio = ((FLOAT)m_size.cx / (FLOAT)m_count);
		FLOAT val = 0;
		TinyRectangle clientRect;
		GetClientRect(hWND, &clientRect);
		TinyClientDC dc(hWND);
		TinyMemDC memDC(dc, TO_CX(clientRect), TO_CY(clientRect));
		memDC.FillRect(&clientRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
		TinyRectangle rectangle = { 0 };
		rectangle.SetSize(TinySize(TO_CX(clientRect), TO_CY(clientRect)));
		for (INT i = 0, cx = 0; cx < m_count; i += m_multiplier, cx++)
		{
			FLOAT wFS = 0.0F;
			for (INT b = 0; b < m_multiplier; b++)
			{
				wFS += wFFT[i + b];
			}
			wFS = (wFS * (FLOAT)log((FLOAT)(cx + 2)));
			if (wFS > 1.0F)
			{
				wFS = 1.0F;
			}
			if (wFS >= (m_oldFFT[i] - m_decay))
			{
				m_oldFFT[i] = wFS;
			}
			else
			{
				m_oldFFT[i] -= m_decay;
				if (m_oldFFT[i] < 0)
				{
					m_oldFFT[i] = 0;
				}
				wFS = m_oldFFT[i];
			}
			DrawSpectrumAnalyserBar(memDC, rectangle, (INT)val, m_size.cy, (INT)wRatio - 1, (INT)(wFS * m_size.cy));
			val += wRatio;
		}
		memDC.Render(clientRect, clientRect, FALSE);
		return TRUE;
	}

	void AudioAnalyser::DrawSpectrumAnalyserBar(TinyMemDC& memDC, TinyRectangle& rectangle, INT x, INT y, INT cx, INT cy)
	{
		for (INT i = y; i >= y - cy; i -= 1)
		{
			rectangle.left = x;
			rectangle.right = rectangle.left + cx;
			rectangle.top = i;
			rectangle.bottom = rectangle.top + 1;
			memDC.FillRect(&rectangle, (HBRUSH)GetStockObject(GRAY_BRUSH));
		}
	}
}
