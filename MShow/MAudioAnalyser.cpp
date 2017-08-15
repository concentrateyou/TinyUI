#include "stdafx.h"
#include "MAudioAnalyser.h"

namespace MShow
{
	MAudioAnalyser::MAudioAnalyser()
		:m_hWND(NULL),
		m_sampleSize(DEFAULT_AUDIO_SAMPLE_SIZE),
		m_sampleSizeFFT(FFT_SAMPLE_SIZE),
		m_decay(DEFAULT_SPECTRUM_ANALYSER_DECAY)
	{
	}

	MAudioAnalyser::~MAudioAnalyser()
	{
	}
	BOOL MAudioAnalyser::Initialize(HWND hWND)
	{
		if (!m_wFFT.Initialize(m_sampleSizeFFT))
			return FALSE;
		m_oldFFT.Reset(new FLOAT[m_sampleSizeFFT]);
		if (!m_oldFFT)
			return FALSE;
		TinyRectangle s;
		::GetClientRect(hWND, &s);
		m_size = s.Size();
		m_samples.Reset(new FLOAT[m_sampleSize]);
		if (!m_samples)
			return FALSE;
		m_hWND = hWND;
		return TRUE;
	}
	BOOL MAudioAnalyser::DrawGradient(HDC hDC, const RECT* pRect, const DWORD* cl, INT iNum, DWORD dwMode)
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
	void MAudioAnalyser::Process(BYTE* bits, LONG size)
	{
		INT samples = (size / 16 * 8) >> 1;//默认单采样16位
		ASSERT(m_sampleSize == samples / 2);
		INT offset = 0;
		for (INT i = 0;i < samples;i++)
		{
			FLOAT valueL = (FLOAT)((bits[offset + 1] << 8) + bits[offset]) / 32767.0F;
			FLOAT valueR = (FLOAT)((bits[offset + 3] << 8) + bits[offset + 2]) / 32767.0F;
			m_samples[i] = (valueL + valueR) / 2.0F;
			offset += 4;
		}
		FLOAT* wFFT = m_wFFT.Calculate(m_samples, samples);
		//绘制频谱
		TinyWindowDC dc(m_hWND);
		TinyMemDC memDC(dc, m_size.cx, m_size.cy);
		RECT rect = { 0, 0, m_size.cx, m_size.cy };
		memDC.FillRect(&rect, ((HBRUSH)GetStockObject(BLACK_BRUSH)));
		DWORD colors[] = { RGB(255,0,0), RGB(0,255,0), RGB(10,10,10), RGB(0,0,255) };
		DrawGradient(memDC, &rect, colors, 4, GRADIENT_FILL_RECT_V);
		for (INT i = m_size.cy;i >= 0;i--)
		{
			if (i % 2 == 0)
				continue;
			MoveToEx(memDC, 0, i, NULL);
			LineTo(memDC, m_size.cx, i);
		}
		INT multiplier = (FLOAT)((m_sampleSizeFFT / 2) / DEFAULT_SPECTRUM_ANALYSER_BAND_COUNT);
		for (INT i = 0, value = 0; value < DEFAULT_SPECTRUM_ANALYSER_BAND_COUNT; i += multiplier, value++)
		{
			FLOAT wFs = 0.F;
			for (INT j = 0; j < multiplier; j++)
			{
				wFs += wFFT[i + j];
			}
			wFs = (wFs * (FLOAT)log(value + 2.0F));
			if (wFs > 0.005F && wFs < 0.009F)
				wFs *= 9.0F * PI;
			else if (wFs > 0.01F && wFs < 0.1F)
				wFs *= 3.0F * PI;
			else if (wFs > 0.1F && wFs < 0.5F)
				wFs *= PI;
			if (wFs > 1.0F)
			{
				wFs = 0.9F;
			}

			if (wFs >= (m_oldFFT[i] - m_decay))
			{
				m_oldFFT[i] = wFs;
			}
			else
			{
				m_oldFFT[i] -= wSadfrr;
				if (m_oldFFT[i] < 0)
				{
					m_oldFFT[i] = 0;
				}
				wFs = m_oldFFT[i];
			}
			c += wBw;
		}
	}
}

