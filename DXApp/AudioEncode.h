#pragma once
#include "DXFramework.h"
#include "AudioCapture.h"
#include "FaacEncode.h"
#include "RTMPClient.h"
using namespace TinyUI::IO;
using namespace Media;

class AudioEncode
{
public:
	AudioEncode();
	virtual ~AudioEncode();
	BOOL				Initialize(const AudioCapture::Name& name, const AudioCaptureParam& param);
	BOOL				Open(DWORD dwAudioRate);
	BOOL				Close();
	BOOL				Run();
	FaacEncode*			GetEncode();
	AudioCapture*		GetCapture();
	AudioCaptureParam*	GetParam();
private:
	void				OnAudio(BYTE* bits, LONG size, FLOAT ts, LPVOID ps);
private:
	BOOL								m_bFull;
	LONG								m_ts;
	FaacEncode							m_aac;
	AudioCapture						m_capture;
	AudioCapture::Name					m_deviceName;
	AudioCaptureParam					m_audioParam;
	TinyPerformanceTimer				m_timer;
	LONG								m_size;
	TinyScopedArray<BYTE>				m_bits;
	RawSampleQueue						m_queue;
	TinyEvent							m_close;
	TinyUI::Callback<void(BYTE*, LONG, FLOAT, LPVOID)> m_audioCB;
};

