#pragma once
#include "DXFramework.h"
#include "AudioCapture.h"
#include "FaacEncode.h"
#include "RTMPClient.h"
#include "Media/TinyMFResampler.h"
#include "Media/TinyWave.h"
#include "IO/TinyIO.h"
#include "Resampler.h"
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
	void				OnResampleDataAvailable(BYTE* bits, LONG size, LPVOID lpParameter);
	void				OnDataAvailable(BYTE* bits, LONG size, DWORD dwFlag, LPVOID lpParameter);
	void				OnAudio(BYTE* bits, LONG size, FLOAT ts, LPVOID ps);
private:
	DWORD				m_dwINC;
	FaacEncode			m_aac;
	AudioCapture		m_capture;
	AudioCapture::Name	m_name;
	AudioCaptureParam	m_audioParam;
	Media::TinyWASAPIAudioCapture	m_wasCapture;
	TinyUI::Callback<void(BYTE*, LONG, LPVOID)>			m_resampleCB;
	TinyUI::Callback<void(BYTE*, LONG, DWORD, LPVOID)>	m_wasCB;
	TinyUI::Callback<void(BYTE*, LONG, FLOAT, LPVOID)>	m_audioCB;
	/*TinyFile					m_rawFile;
	Media::TinyWaveFile			m_waveFile;*/
	Media::TinyMFResampler		m_resampler;
	//Resampler						m_resampler;
};

