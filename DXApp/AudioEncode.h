#pragma once
#include "DXFramework.h"
#include "AudioInputCapture.h"
#include "FaacEncode.h"
#include "RTMPClient.h"
#include "Media/TinyMFResampler.h"
#include "Media/TinyWave.h"
#include "IO/TinyIO.h"
#include "FFmpegResampler.h"
using namespace TinyUI::IO;
using namespace DShow;

class AudioEncode
{
public:
	AudioEncode();
	virtual ~AudioEncode();
	BOOL				Initialize(const AudioInputCapture::Name& name, const AudioCaptureParam& param);
	BOOL				Open(DWORD dwAudioRate);
	BOOL				Close();
	BOOL				Encode();
	FaacEncode*			GetEncode();
	AudioInputCapture*	GetCapture();
	AudioCaptureParam*	GetParam();
private:
	void				OnAudio(BYTE* bits, LONG size, FLOAT ts, LPVOID ps);
private:
	DWORD					m_dwOffset;
	DWORD					m_dwINC;
	FaacEncode				m_aac;
	AudioInputCapture		m_capture;
	AudioInputCapture::Name	m_name;
	AudioCaptureParam		m_audioParam;
	TinyUI::Callback<void(BYTE*, LONG, FLOAT, LPVOID)>	m_audioCB;
};

