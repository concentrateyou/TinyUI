#include "stdafx.h"
#include "AudioEncode.h"


AudioEncode::AudioEncode()
	:m_size(0),
	m_ts(0),
	m_bFull(FALSE)
{
}


AudioEncode::~AudioEncode()
{
}

FaacEncode*	AudioEncode::GetEncode()
{
	return &m_aac;
}

AudioCapture* AudioEncode::GetCapture()
{
	return &m_capture;
}

AudioCaptureParam*	AudioEncode::GetParam()
{
	return &m_audioParam;
}

BOOL AudioEncode::Initialize(const AudioCapture::Name& name, const AudioCaptureParam& param)
{
	m_deviceName = name;
	m_audioParam = param;
	m_audioCB = BindCallback(&AudioEncode::OnAudio, this);
	BOOL bRes = m_capture.Initialize(m_deviceName, m_audioCB);
	if (!bRes)
		return FALSE;
	bRes = m_capture.Allocate(param);
	if (!bRes)
		return FALSE;
	return TRUE;
}

BOOL AudioEncode::Open(DWORD dwAudioRate)
{
	BOOL bRes = m_aac.Open(m_audioParam.GetFormat(), (DWORD)dwAudioRate);
	if (!bRes)
		return FALSE;
	return TRUE;
}

BOOL AudioEncode::Run()
{
	m_capture.Start();
}

BOOL AudioEncode::Close(DWORD dwMS)
{
	m_capture.Uninitialize();
	m_aac.Close();
}

void AudioEncode::OnClose()
{
	m_capture.Uninitialize();
	m_aac.Close();
}

void AudioEncode::OnAudio(BYTE* bits, LONG size, FLOAT ts, LPVOID ps)
{
	m_aac.Encode(bits, size);
}