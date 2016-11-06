#pragma once
#include "DXFramework.h"
#include "AudioCapture.h"
#include "FaacEncode.h"
#include "RTMPClient.h"
#include "Media/TinyMFResampler.h"
#include "Media/TinyWave.h"
#include "IO/TinyIO.h"
#include "FFmpegResampler.h"
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
	void				OnResampleDataAvailable16(BYTE* bits, LONG size, LPVOID lpParameter);
	void				OnResampleDataAvailable16F(FLOAT* bits, LONG count, LPVOID lpParameter);
	void				OnDataAvailable(BYTE* bits, LONG size, LPVOID lpParameter);
	void				OnAudio(BYTE* bits, LONG size, FLOAT ts, LPVOID ps);
private:
	DWORD				m_dwOffset;
	DWORD				m_dwINC;
	FaacEncode			m_aac;
	AudioCapture		m_capture;
	AudioCapture::Name	m_name;
	AudioCaptureParam	m_audioParam;
	Media::TinyWASAPIAudioCapture	m_wasCapture;
	FFmpegResampler		m_resampler;
	//TinyMFResampler		m_resampler;
	//Resampler			m_resampler;
	BYTE				m_buffer[4096];
	TinyUI::Callback<void(BYTE*, LONG, LPVOID)>			m_resample16CB;
	TinyUI::Callback<void(FLOAT*, LONG, LPVOID)>		m_resample16CBF;
	TinyUI::Callback<void(BYTE*, LONG, LPVOID)>			m_wasCB;
	TinyUI::Callback<void(BYTE*, LONG, FLOAT, LPVOID)>	m_audioCB;
};

