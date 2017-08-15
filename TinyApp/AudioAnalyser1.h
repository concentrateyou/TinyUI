#pragma once
#include "FFT1.h"
using namespace Decode;

namespace MShow
{
#define DEFAULT_SAMPLE_SIZE  1024
#define DEFAULT_AUDIO_SAMPLE_SIZE 1024
#define DEFAULT_SPECTRUM_ANALYSER_BAND_COUNT 60
#define DEFAULT_SPECTRUM_ANALYSER_DECAY 0.05F
#define DEFAULT_SPECTRUM_ANALYSER_PEAK_DELAY 5 
#define DEFAULT_SPECTRUM_ANALYSER_PEAK_DELAY_FPS_RATIO 0.4F
#define DEFAULT_SPECTRUM_ANALYSER_PEAK_DELAY_FPS_RATIO_RANGE 0.1F
#define MIN_SPECTRUM_ANALYSER_DECAY	0.02F
#define MAX_SPECTRUM_ANALYSER_DECAY 0.08F
#define DEFAULT_SPECTRUM_ANALYSER_FFT_SAMPLE_SIZE 512
	/// <summary>
	/// 音频频谱分析(FFT),默认只处理双声道数据
	/// </summary>
	/*class AudioAnalyser
	{
		DISALLOW_COPY_AND_ASSIGN(AudioAnalyser)
	public:
		AudioAnalyser();
		virtual ~AudioAnalyser();
		BOOL Initialize(HWND hWND);
		void Process(BYTE* bits, LONG size);
	private:
		BOOL DrawGradient(HDC hDC, const RECT* pRect, const DWORD* cl, INT iNum, DWORD dwMode);
		void AudioAnalyser::DrawAnalyserBar(TinyMemDC& memDC, TinyMemDC& memDC1, INT pX, INT pY, INT pWidth, INT pHeight, INT band);
	public:
		FLOAT						m_decay;
		INT							m_sampleSize;
		INT							m_sampleSizeFFT;
		HWND						m_hWND;
		FFT							m_wFFT;
		TinySize					m_size;
		TinyScopedArray<FLOAT>		m_oldFFT;
		TinyScopedArray<FLOAT>		m_samples;
	};*/
	class AudioAnalyser1
	{
	private:
		INT m_SampleSize;
		LONG m_FpsAsNS;
		LONG m_DesiredFpsAsNS;
		FLOAT* m_Left;
		FLOAT* m_Right;
		INT m_position;
		INT m_offset;
		INT m_sampleType;
		INT m_channelMode;
		App::MyFFT* m_FFT;
		INT m_width;
		INT m_height;
		INT* m_peaks;
		INT* m_peaksDelay;
		FLOAT* m_oldFFT;
		INT m_saFFTSampleSize;
		INT m_saBands;
		FLOAT m_saDecay;
		FLOAT m_saMultiplier;
		INT m_peakDelay;
		BOOL m_peaksEnabled;
		HDC m_hdcScreen;
		HDC m_hdcMem;
		HDC m_hdcMem1;
		HBITMAP m_hbmMem;
		HBITMAP m_hbmMem1;
		HBITMAP m_hOld;
		HBITMAP m_hOld1;
		HBRUSH m_hbrush;
		HBRUSH m_hbrush1;
		HBRUSH m_hbrushOld;
		HBRUSH m_hbrushOld1;
		INT m_winwidth, m_winheight;
		INT m_barOffset = 1;
		void drawSpectrumAnalyserBar(RECT* pRect, int pX, int pY, int pWidth, int pHeight, int band);
		BOOL DrawGradient(HDC hDC, const RECT* pRect, const DWORD* cl, INT iNum, DWORD dwMode);
		void prepare();
	public:
		AudioAnalyser1();
		~AudioAnalyser1(void);
		void Initialize(HWND hWND);
		void Process(BYTE* bits, LONG size, float pFrameRateRatioHint);
	};
}


