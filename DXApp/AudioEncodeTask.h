#pragma once
#include "DXFramework.h"
#include "AudioCapture.h"
#include "FaacEncode.h"
#include "RTMPClient.h"
using namespace TinyUI::IO;
using namespace Media;

class AudioEncodeTask : public TinyTaskBase
{
public:
	AudioEncodeTask();
	virtual ~AudioEncodeTask();
	BOOL				Initialize(const AudioCapture::Name& name, const AudioCaptureParam& param);
	BOOL				Open(DWORD dwAudioRate);
	BOOL				Submit();
	BOOL				Close(DWORD dwMS = INFINITE) OVERRIDE;
	FaacEncode*			GetEncode();
	AudioCapture*		GetCapture();
	AudioCaptureParam*	GetParam();
private:
	void			OnAudio(BYTE* bits, LONG size, FLOAT ts, LPVOID ps);
	void			OnMessagePump();
	void			OnClose();
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

