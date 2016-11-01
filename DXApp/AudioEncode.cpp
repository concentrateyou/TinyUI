#include "stdafx.h"
#include "AudioEncode.h"


AudioEncode::AudioEncode()
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
	m_name = name;
	m_audioParam = param;
	m_audioCB = BindCallback(&AudioEncode::OnAudio, this);
	BOOL bRes = m_capture.Initialize(m_name, m_audioCB);
	if (!bRes)
		return FALSE;
	bRes = m_capture.Allocate(param);
	if (!bRes)
		return FALSE;
	m_wasCB = BindCallback(&AudioEncode::OnDataAvailable, this);
	m_wasCapture.SetCallback(m_wasCB);
	return TRUE;
}

BOOL AudioEncode::Open(DWORD dwAudioRate)
{
	m_wasCapture.SetOutputFormat(m_audioParam.GetFormat());
	BOOL bRes = m_aac.Open(m_audioParam.GetFormat(), (DWORD)dwAudioRate);
	if (!bRes)
		return FALSE;
	bRes = m_wasCapture.Open();
	if (!bRes)
		return FALSE;
	bRes = m_capture.Start();
	if (!bRes)
		return FALSE;
	bRes = m_wasCapture.Start();
	if (!bRes)
		return FALSE;
	return TRUE;
}

BOOL AudioEncode::Encode()
{
	return m_capture.Start();
}

BOOL AudioEncode::Close()
{
	m_capture.Uninitialize();
	m_aac.Close();
	return TRUE;
}

void AudioEncode::OnDataAvailable(BYTE* bits, LONG size, LPVOID lpParameter)
{
	m_aac.Encode(bits, size, m_dwINC);
}

void AudioEncode::OnAudio(BYTE* bits, LONG size, FLOAT ts, LPVOID ps)
{
	m_aac.Encode(bits, size, m_dwINC);
}