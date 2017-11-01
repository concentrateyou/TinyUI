#pragma once
#include "Media/TinyWave.h"
#include "Media/TinyAudioDSPCapture.h"
#include "Media/TinyMFResampler.h"
#include "Common/TinyTime.h"
#include "Common/TinyCore.h"
using namespace TinyUI;
using namespace TinyUI::Media;

class AudioDSP
{
public:
	AudioDSP();
	~AudioDSP();
	BOOL Initialize();
private:
	void OnDSP(BYTE* bits, LONG size, LPVOID lpParameter);
	void OnAudio(BYTE* bits, LONG size, LPVOID lpParameter);
	void OnTimer();
private:
	TinyLock				m_lock;
	TinyWaveFile			m_waveFile;
	TinyAudioDSPCapture		m_audioDSP;
	TinyMFResampler			m_resampler;
	TinyBufferArray<BYTE>	m_buffer;
	TinyPerformanceTime		m_timeQPC;
	TinyPerformanceTimer	m_timer;
	BYTE					m_data[4096];
};

