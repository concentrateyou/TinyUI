#pragma once
#include "DXFramework.h"
#include "AudioCapture.h"
#include "aacEncode.h"
#include "RTMPClient.h"
using namespace TinyUI::IO;
using namespace Media;

class AudioEncodeTask : public TinyTaskBase
{
public:
	AudioEncodeTask();
	virtual ~AudioEncodeTask();
	BOOL			Initialize(const AudioCapture::Name& name, const AudioCaptureParam& param);
	BOOL			Open(DWORD dwAudioRate);
	BOOL			Submit();
	void			Exit() OVERRIDE;
	aacEncode*		GetEncode();
	AudioCapture*	GetCapture();
private:
	void			OnAudio(BYTE* bits, LONG size,LPVOID ps);
	void			OnMessagePump();
	void			OnExit();
private:
	aacEncode							m_aac;
	AudioCapture						m_capture;
	AudioCapture::Name					m_deviceName;
	AudioCaptureParam					m_audioParam;
	TinyPerformanceTimer				m_timer;
	LONG								m_size;
	TinyScopedArray<BYTE>				m_bits;
	IO::TinyRingQueue					m_queue;
	TinyUI::Callback<void(BYTE*, LONG,LPVOID)> m_audioCB;
};

