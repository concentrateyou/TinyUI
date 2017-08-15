#include "stdafx.h"
#include "AudioAnalyser1.h"

namespace MShow
{
	AudioAnalyser1::AudioAnalyser1()
	{

	}

	void AudioAnalyser1::Initialize(HWND hWND)
	{
		/* digital signal process */
		m_SampleSize = DEFAULT_SAMPLE_SIZE;
		m_Left = new FLOAT[DEFAULT_SAMPLE_SIZE];
		m_Right = new FLOAT[DEFAULT_SAMPLE_SIZE];

		memset(m_Left, 0, DEFAULT_SAMPLE_SIZE);
		memset(m_Right, 0, DEFAULT_SAMPLE_SIZE);
		RECT s;
		GetClientRect(hWND, &s);
		/* spectrum analyser */
		m_width = 300;
		m_height = 180;
		m_saDecay = DEFAULT_SPECTRUM_ANALYSER_DECAY;
		m_saFFTSampleSize = DEFAULT_SPECTRUM_ANALYSER_FFT_SAMPLE_SIZE;
		m_saBands = DEFAULT_SPECTRUM_ANALYSER_BAND_COUNT;
		m_FFT = new App::MyFFT(m_saFFTSampleSize);
		m_oldFFT = new FLOAT[m_saFFTSampleSize];
		m_saMultiplier = (FLOAT)((m_saFFTSampleSize / 2) / m_saBands);
		memset(m_oldFFT, 0, m_saFFTSampleSize);

		/* draw spectrum analyser */
		RECT rect;
		GetClientRect(hWND, &rect);
		m_winwidth = rect.right - rect.left;
		m_winheight = rect.bottom - rect.top;

		m_hdcScreen = GetWindowDC(hWND);
		m_hdcMem = CreateCompatibleDC(m_hdcScreen);
		m_hbmMem = CreateCompatibleBitmap(m_hdcScreen, m_winwidth, m_winheight);
		m_hbrush = CreateSolidBrush(RGB(0, 0, 0));
		m_hbrush1 = CreateSolidBrush(RGB(125, 125, 125));
		m_hOld = (HBITMAP)SelectObject(m_hdcMem, m_hbmMem);
		m_hbrushOld = (HBRUSH)SelectObject(m_hdcMem, m_hbrush);
		m_hbrushOld1 = (HBRUSH)SelectObject(m_hdcMem, m_hbrush1);

		prepare();
	}

	AudioAnalyser1::~AudioAnalyser1(void)
	{
		if (m_Left != NULL)
			delete[] m_Left;

		if (m_Right != NULL)
			delete[] m_Right;

		if (m_peaks != NULL)
			delete[] m_peaks;

		if (m_peaksDelay != NULL)
			delete[] m_peaksDelay;

		if (m_oldFFT != NULL)
			delete[] m_oldFFT;

		if (m_FFT != NULL)
			delete m_FFT;

		m_Left = NULL;
		m_Right = NULL;
		m_peaks = NULL;
		m_peaksDelay = NULL;
		m_oldFFT = NULL;
		m_FFT = NULL;

		/* draw spectrum analyser */
		SelectObject(m_hdcMem, m_hOld);
		SelectObject(m_hdcMem1, m_hOld1);
		SelectObject(m_hdcMem, m_hbrushOld);
		SelectObject(m_hdcMem, m_hbrushOld1);
		DeleteObject(m_hbmMem);
		DeleteObject(m_hbmMem1);
		DeleteObject(m_hbrush);
		DeleteObject(m_hbrush1);
		DeleteDC(m_hdcMem);
		DeleteDC(m_hdcMem1);
	}

	void AudioAnalyser1::Process(BYTE* bits, LONG size, float pFrameRateRatioHint)
	{
		int b = 0;
		for (int a = 0; a < m_SampleSize;a++)
		{
			m_Left[a] = (float)((bits[b + 1] << 8) + bits[b]) / 32767.0F;
			m_Right[a] = (float)((bits[b + 3] << 8) + bits[b + 2]) / 32767.0F;
			b += 4;
		}
		for (int a = 0; a < m_SampleSize; a++)
		{
			m_Left[a] = (m_Left[a] + m_Right[a]) / 2.0f;
		}

		float c = 0;
		float pFrrh = pFrameRateRatioHint;
		float* wFFT = m_FFT->Calculate(m_Left, m_SampleSize);
		float wSadfrr = (m_saDecay * pFrrh);
		float wBw = ((float)m_width / (float)m_saBands);

		RECT rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = rect.left + m_winwidth;
		rect.bottom = rect.top + m_winheight;

		FillRect(m_hdcMem, &rect, m_hbrush);
		for (int a = 0, bd = 0; bd < m_saBands; a += (INT)m_saMultiplier, bd++)
		{
			float wFs = 0;
			for (int b = 0; b < m_saMultiplier; b++)
			{
				wFs += wFFT[a + b];
			}
			wFs = (wFs * (float)log((FLOAT)(bd + 2)));
			if (wFs > 1.0f)
			{
				wFs = 1.0f;
			}
			if (wFs >= (m_oldFFT[a] - wSadfrr))
			{
				m_oldFFT[a] = wFs;
			}
			else
			{
				m_oldFFT[a] -= wSadfrr;
				if (m_oldFFT[a] < 0)
				{
					m_oldFFT[a] = 0;
				}
				wFs = m_oldFFT[a];
			}
			drawSpectrumAnalyserBar(&rect, (int)c, m_height, (int)wBw - 1, (int)(wFs * m_height), bd);
			c += wBw;
		}
		BitBlt(m_hdcScreen, 8, 49, m_winwidth, m_winheight, m_hdcMem, 0, 0, SRCCOPY);
	}
	BOOL AudioAnalyser1::DrawGradient(HDC hDC, const RECT* pRect, const DWORD* cl, INT iNum, DWORD dwMode)
	{
		INT cx;
		INT cy;
		TRIVERTEX *pTRIVERTEX = NULL;
		GRADIENT_RECT  *pGRECT = NULL;
		if (cl == NULL || iNum < 1 || pRect == NULL || dwMode == GRADIENT_FILL_TRIANGLE)
		{
			::SetLastError(ERROR_INVALID_PARAMETER);
			return TRUE;
		}
		if (iNum == 1)
		{
			HBRUSH hBrush = CreateSolidBrush(cl[0]);
			SelectObject(hDC, hBrush);
			FillRect(hDC, pRect, hBrush);
			DeleteObject(hBrush);
			return TRUE;
		}

		pTRIVERTEX = new TRIVERTEX[iNum * 2 - 2];
		pGRECT = new GRADIENT_RECT[iNum];
		cx = pRect->right - pRect->left;
		cy = pRect->bottom - pRect->top;
		for (int i = 0; i < iNum - 1; i++)
		{
			if (dwMode == GRADIENT_FILL_RECT_V)
			{
				pTRIVERTEX[i * 2].x = pRect->left;
				pTRIVERTEX[i * 2].y = pRect->top + cy / (iNum - 1) * i;

				pTRIVERTEX[i * 2 + 1].x = pRect->right;
				pTRIVERTEX[i * 2 + 1].y = pRect->top + cy / (iNum - 1) * (i + 1);
			}
			else if (dwMode == GRADIENT_FILL_RECT_H)
			{
				pTRIVERTEX[i * 2].x = pRect->left + cx / (iNum - 1) * i;
				pTRIVERTEX[i * 2].y = pRect->top;

				pTRIVERTEX[i * 2 + 1].x = pRect->left + cx / (iNum - 1) * (i + 1);
				pTRIVERTEX[i * 2 + 1].y = pRect->bottom;
			}
			pTRIVERTEX[i * 2].Red = (WORD)GetRValue((cl[i])) << 8;
			pTRIVERTEX[i * 2].Green = (WORD)GetGValue((cl[i])) << 8;
			pTRIVERTEX[i * 2].Blue = (WORD)GetBValue((cl[i])) << 8;
			pTRIVERTEX[i * 2].Alpha = 0x0000;
			pTRIVERTEX[i * 2 + 1].Red = (WORD)GetRValue((cl[i + 1])) << 8;
			pTRIVERTEX[i * 2 + 1].Green = (WORD)GetGValue((cl[i + 1])) << 8;
			pTRIVERTEX[i * 2 + 1].Blue = (WORD)GetBValue((cl[i + 1])) << 8;
			pTRIVERTEX[i * 2 + 1].Alpha = 0x0000;
			pGRECT[i].UpperLeft = i * 2;
			pGRECT[i].LowerRight = i * 2 + 1;
		}
		BOOL bRet = ::GradientFill(hDC, pTRIVERTEX, iNum * 2, pGRECT, iNum - 1, dwMode);
		SAFE_DELETE_ARRAY(pTRIVERTEX);
		SAFE_DELETE_ARRAY(pGRECT);
		return bRet;
	}

	void AudioAnalyser1::drawSpectrumAnalyserBar(RECT* pRect, int pX, int pY, int pWidth, int pHeight, int band)
	{
		float c = 0;
		for (int a = pY; a >= pY - pHeight; a -= m_barOffset) 
		{
			pRect->left = pX;
			pRect->right = pRect->left + pWidth;
			pRect->top = a;
			pRect->bottom = pRect->top + 1;
			FillRect(m_hdcMem, pRect, m_hbrush1);
		}
		/* draw gradient bar */
		//BitBlt(m_hdcMem, pX, pY - pHeight, pWidth, pHeight, m_hdcMem1, pX, pY - pHeight, SRCCOPY);

		/*	if (m_peaksEnabled == TRUE)
			{
				if (pHeight > m_peaks[band])
				{
					m_peaks[band] = pHeight;
					m_peaksDelay[band] = m_peakDelay;
				}
				else
				{
					m_peaksDelay[band]--;
					if (m_peaksDelay[band] < 0)
					{
						m_peaks[band]--;
					}
					if (m_peaks[band] < 0)
					{
						m_peaks[band] = 0;
					}
				}

				RECT rect = { 0 };
				rect.left = pX;
				rect.right = rect.left + pWidth;
				rect.top = pY - m_peaks[band];
				rect.bottom = rect.top + 1;
				FillRect(m_hdcMem, &rect, m_hbrush1);
			}*/
	}

	void AudioAnalyser1::prepare()
	{
		/* draw gradient */
		RECT rect = { 0, 0, m_width, m_height };
		DWORD colors[] = { RGB(255,0,0), RGB(0,255,0), RGB(10,10,10), RGB(0,0,255) };
		DrawGradient(m_hdcMem1, &rect, colors, 4, GRADIENT_FILL_RECT_V);

		/* draw grid */
		for (int i = m_height;i >= 0;i--)
		{
			if (i % 2 == 0)
				continue;

			MoveToEx(m_hdcMem1, 0, i, NULL);
			LineTo(m_hdcMem1, m_width, i);
		}
	}

	//AudioAnalyser::AudioAnalyser()
	//	:m_hWND(NULL),
	//	m_sampleSize(DEFAULT_AUDIO_SAMPLE_SIZE),
	//	m_sampleSizeFFT(FFT_SAMPLE_SIZE),
	//	m_decay(DEFAULT_SPECTRUM_ANALYSER_DECAY)
	//{
	//}

	//AudioAnalyser::~AudioAnalyser()
	//{
	//}
	//BOOL AudioAnalyser::Initialize(HWND hWND)
	//{
	//	if (!m_wFFT.Initialize(m_sampleSizeFFT))
	//		return FALSE;
	//	m_oldFFT.Reset(new FLOAT[m_sampleSizeFFT]);
	//	if (!m_oldFFT)
	//		return FALSE;
	//	memset(m_oldFFT, 0, m_sampleSizeFFT);
	//	TinyRectangle s;
	//	::GetClientRect(hWND, &s);
	//	m_size = s.Size();
	//	m_samples.Reset(new FLOAT[m_sampleSize]);
	//	if (!m_samples)
	//		return FALSE;
	//	m_hWND = hWND;
	//	return TRUE;
	//}
	//BOOL AudioAnalyser::DrawGradient(HDC hDC, const RECT* pRect, const DWORD* cl, INT iNum, DWORD dwMode)
	//{
	//	INT cx;
	//	INT cy;
	//	TRIVERTEX *pTRIVERTEX = NULL;
	//	GRADIENT_RECT  *pGRECT = NULL;
	//	if (cl == NULL || iNum < 1 || pRect == NULL || dwMode == GRADIENT_FILL_TRIANGLE)
	//	{
	//		::SetLastError(ERROR_INVALID_PARAMETER);
	//		return TRUE;
	//	}
	//	if (iNum == 1)
	//	{
	//		HBRUSH hBrush = CreateSolidBrush(cl[0]);
	//		SelectObject(hDC, hBrush);
	//		FillRect(hDC, pRect, hBrush);
	//		DeleteObject(hBrush);
	//		return TRUE;
	//	}

	//	pTRIVERTEX = new TRIVERTEX[iNum * 2 - 2];
	//	pGRECT = new GRADIENT_RECT[iNum];
	//	cx = pRect->right - pRect->left;
	//	cy = pRect->bottom - pRect->top;
	//	for (int i = 0; i < iNum - 1; i++)
	//	{
	//		if (dwMode == GRADIENT_FILL_RECT_V)
	//		{
	//			pTRIVERTEX[i * 2].x = pRect->left;
	//			pTRIVERTEX[i * 2].y = pRect->top + cy / (iNum - 1) * i;

	//			pTRIVERTEX[i * 2 + 1].x = pRect->right;
	//			pTRIVERTEX[i * 2 + 1].y = pRect->top + cy / (iNum - 1) * (i + 1);
	//		}
	//		else if (dwMode == GRADIENT_FILL_RECT_H)
	//		{
	//			pTRIVERTEX[i * 2].x = pRect->left + cx / (iNum - 1) * i;
	//			pTRIVERTEX[i * 2].y = pRect->top;

	//			pTRIVERTEX[i * 2 + 1].x = pRect->left + cx / (iNum - 1) * (i + 1);
	//			pTRIVERTEX[i * 2 + 1].y = pRect->bottom;
	//		}
	//		pTRIVERTEX[i * 2].Red = (WORD)GetRValue((cl[i])) << 8;
	//		pTRIVERTEX[i * 2].Green = (WORD)GetGValue((cl[i])) << 8;
	//		pTRIVERTEX[i * 2].Blue = (WORD)GetBValue((cl[i])) << 8;
	//		pTRIVERTEX[i * 2].Alpha = 0x0000;
	//		pTRIVERTEX[i * 2 + 1].Red = (WORD)GetRValue((cl[i + 1])) << 8;
	//		pTRIVERTEX[i * 2 + 1].Green = (WORD)GetGValue((cl[i + 1])) << 8;
	//		pTRIVERTEX[i * 2 + 1].Blue = (WORD)GetBValue((cl[i + 1])) << 8;
	//		pTRIVERTEX[i * 2 + 1].Alpha = 0x0000;
	//		pGRECT[i].UpperLeft = i * 2;
	//		pGRECT[i].LowerRight = i * 2 + 1;
	//	}
	//	BOOL bRet = ::GradientFill(hDC, pTRIVERTEX, iNum * 2, pGRECT, iNum - 1, dwMode);
	//	SAFE_DELETE_ARRAY(pTRIVERTEX);
	//	SAFE_DELETE_ARRAY(pGRECT);
	//	return bRet;
	//}
	//void AudioAnalyser::Process(BYTE* bits, LONG size)
	//{
	//	INT samples = (size / 16 * 8) >> 1;//默认单采样16位
	//	ASSERT(m_sampleSize == samples);
	//	INT offset = 0;
	//	for (INT i = 0;i < samples;i++)
	//	{
	//		FLOAT valueL = (FLOAT)((bits[offset + 1] << 8) + bits[offset]) / 32767.0F;
	//		FLOAT valueR = (FLOAT)((bits[offset + 3] << 8) + bits[offset + 2]) / 32767.0F;
	//		m_samples[i] = (valueL + valueR) / 2.0F;
	//		offset += 4;
	//	}
	//	FLOAT* wFFT = m_wFFT.Calculate(m_samples, samples);
	//	FLOAT x = 0;
	//	FLOAT wBw = ((FLOAT)m_size.cx / (FLOAT)DEFAULT_SPECTRUM_ANALYSER_BAND_COUNT);
	//	//绘制频谱
	//	TinyClientDC dc(m_hWND);
	//	TinyMemDC memDC(dc, m_size.cx, m_size.cy);
	//	TinyMemDC memDC1(dc, m_size.cx, m_size.cy);
	//	RECT recnatgle = { 0, 0, m_size.cx, m_size.cy };
	//	memDC.FillRect(&recnatgle, ((HBRUSH)GetStockObject(BLACK_BRUSH)));
	//	DWORD colors[] = { RGB(255,0,0), RGB(0,255,0), RGB(10,10,10), RGB(0,0,255) };
	//	DrawGradient(memDC1, &recnatgle, colors, 4, GRADIENT_FILL_RECT_V);
	//	for (INT i = m_size.cy;i >= 0;i--)
	//	{
	//		if (i % 2 == 0)
	//			continue;
	//		MoveToEx(memDC1, 0, i, NULL);
	//		LineTo(memDC1, m_size.cx, i);
	//	}
	//	INT multiplier = (FLOAT)((m_sampleSizeFFT / 2) / DEFAULT_SPECTRUM_ANALYSER_BAND_COUNT);
	//	for (INT i = 0, value = 0; value < DEFAULT_SPECTRUM_ANALYSER_BAND_COUNT; i += multiplier, value++)
	//	{
	//		FLOAT wFs = 0.F;
	//		for (INT j = 0; j < multiplier; j++)
	//		{
	//			wFs += wFFT[i + j];
	//		}
	//		wFs = (wFs * (FLOAT)log(value + 2.0F));
	//		if (wFs > 0.005F && wFs < 0.009F)
	//			wFs *= 9.0F * PI;
	//		else if (wFs > 0.01F && wFs < 0.1F)
	//			wFs *= 3.0F * PI;
	//		else if (wFs > 0.1F && wFs < 0.5F)
	//			wFs *= PI;
	//		if (wFs > 1.0F)
	//		{
	//			wFs = 0.9F;
	//		}
	//		if (wFs >= (m_oldFFT[i] - m_decay))
	//		{
	//			m_oldFFT[i] = wFs;
	//		}
	//		else
	//		{
	//			m_oldFFT[i] -= m_decay;
	//			if (m_oldFFT[i] < 0)
	//			{
	//				m_oldFFT[i] = 0;
	//			}
	//			wFs = m_oldFFT[i];
	//		}
	//		DrawAnalyserBar(memDC, memDC1, (INT)x, m_size.cy, (INT)wBw - 1, (INT)(wFs * m_size.cy), value);
	//		x += wBw;
	//	}
	//	memDC.Render(recnatgle, recnatgle, FALSE);
	//}

	//void AudioAnalyser::DrawAnalyserBar(TinyMemDC& memDC, TinyMemDC& memDC1, INT pX, INT pY, INT pWidth, INT pHeight, INT band)
	//{
	//	BitBlt(memDC.Handle(), pX, pY - pHeight, pWidth, pHeight, memDC1.Handle(), pX, pY - pHeight, SRCCOPY);
	//}
}

