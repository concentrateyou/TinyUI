#pragma once
#include "MShowCommon.h"
#include "FFT.h"
using namespace Decode;

namespace MShow
{
#define DEFAULT_AUDIO_SAMPLE_SIZE 1024
#define DEFAULT_SPECTRUM_ANALYSER_BAND_COUNT 30
#define DEFAULT_SPECTRUM_ANALYSER_DECAY 0.05F
#define DEFAULT_SPECTRUM_ANALYSER_PEAK_DELAY 5 
#define DEFAULT_SPECTRUM_ANALYSER_PEAK_DELAY_FPS_RATIO 0.4F
#define DEFAULT_SPECTRUM_ANALYSER_PEAK_DELAY_FPS_RATIO_RANGE 0.1F
#define MIN_SPECTRUM_ANALYSER_DECAY	0.02F
#define MAX_SPECTRUM_ANALYSER_DECAY 0.08F
	/// <summary>
	/// 音频频谱分析(FFT),默认只处理双声道数据
	/// </summary>
	class MAudioAnalyser
	{
		DISALLOW_COPY_AND_ASSIGN(MAudioAnalyser)
	public:
		MAudioAnalyser();
		virtual ~MAudioAnalyser();
		BOOL Initialize(HWND hWND, WAVEFORMATEX& waveFMT);
		void Process(BYTE* bits, LONG size);
	private:
		BOOL DrawGradient(HDC hDC, const RECT* pRect, const DWORD* cl, INT iNum, DWORD dwMode);
	public:
		HWND						m_hWND;
		FFT							m_wFFT;
		TinySize					m_size;
		WAVEFORMATEX				m_waveFMT;
		TinyScopedArray<FLOAT>		m_samples;
	};
}


