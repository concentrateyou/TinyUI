#pragma once
#include "Common.h"
#include "FFT.h"
using namespace TinyUI;

namespace Decode
{
#define DEFAULT_SAMPLE_SIZE  1024
#define DEFAULT_SPECTRUM_ANALYSER_BAND_COUNT 60
#define DEFAULT_SPECTRUM_ANALYSER_DECAY 0.05F
#define MIN_SPECTRUM_ANALYSER_DECAY	0.02F
#define MAX_SPECTRUM_ANALYSER_DECAY 0.08F
#define DEFAULT_SPECTRUM_ANALYSER_FFT_SAMPLE_SIZE 512
	/// <summary>
	/// 频谱分析目前只支持双声道立体声
	/// </summary>
	class AudioAnalyser
	{
		DISALLOW_COPY_AND_ASSIGN(AudioAnalyser)
	public:
		AudioAnalyser();
		virtual ~AudioAnalyser();
	public:
		BOOL Initialize();
		BOOL Process(HWND hWND, BYTE* bits, LONG size);
	private:
		void DrawSpectrumAnalyserBar(TinyMemDC& memDC, TinyRectangle& rectangle, INT x, INT y, INT cx, INT cy);
	private:
		TinySize				m_size;//BAR大小
		FFT						m_audioFFT;
		FLOAT					m_decay;
		INT						m_count;
		INT						m_multiplier;
		INT						m_sampleSize;
		TinyScopedArray<FLOAT>	m_samples;
		TinyScopedArray<FLOAT>	m_oldFFT;
	};
}


