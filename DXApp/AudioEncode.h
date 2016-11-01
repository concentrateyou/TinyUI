#pragma once
#include "DXFramework.h"
#include "AudioCapture.h"
#include "FaacEncode.h"
#include "RTMPClient.h"
#include "Media/TinyWave.h"
using namespace TinyUI::IO;
using namespace DShow;

class AudioEncode
{
public:
	AudioEncode();
	virtual ~AudioEncode();
	BOOL				Initialize(const AudioCapture::Name& name, const AudioCaptureParam& param);
	BOOL				Open(DWORD dwAudioRate);
	BOOL				Close();
	BOOL				Encode();
	FaacEncode*			GetEncode();
	AudioCapture*		GetCapture();
	AudioCaptureParam*	GetParam();
private:
	void				OnDataAvailable(BYTE* bits, LONG size, LPVOID lpParameter);
	void				OnAudio(BYTE* bits, LONG size, FLOAT ts, LPVOID ps);
private:
	TinyLock			m_lock;
	DWORD				m_dwINC;
	FaacEncode			m_aac;
	AudioCapture		m_capture;
	AudioCapture::Name	m_name;
	AudioCaptureParam	m_audioParam;
	Media::TinyWASAPIAudioCapture	m_wasCapture;
	Media::TinyWaveFile				m_waveFile;
	TinyUI::Callback<void(BYTE*, LONG, LPVOID)> m_wasCB;
	TinyUI::Callback<void(BYTE*, LONG, FLOAT, LPVOID)> m_audioCB;


};

