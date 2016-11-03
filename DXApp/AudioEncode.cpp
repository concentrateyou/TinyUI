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
	m_wasCapture.Initialize(m_wasCB);
	return TRUE;
}

BOOL AudioEncode::Open(DWORD dwAudioRate)
{
	BOOL bRes = m_aac.Open(m_audioParam.GetFormat(), (DWORD)dwAudioRate);
	if (!bRes)
		return FALSE;
	bRes = m_wasCapture.Open();
	if (!bRes)
		return FALSE;
	m_resampleCB = BindCallback(&AudioEncode::OnResampleDataAvailable, this);
	bRes = m_resampler.Open(m_wasCapture.GetInputFormat(), &m_audioParam.GetFormat(), m_resampleCB);
	if (!bRes)
		return FALSE;
	bRes = m_capture.Start();
	if (!bRes)
		return FALSE;
	bRes = m_wasCapture.Start();
	if (!bRes)
		return FALSE;
	m_waveFile.Create("D:\\1234.wav", &m_audioParam.GetFormat());
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
	m_wasCapture.Close();
	m_resampler.Close();
	m_waveFile.Close();
	return TRUE;
}
void AudioEncode::OnResampleDataAvailable(BYTE* bits, LONG size, LPVOID lpParameter)
{
	//m_waveFile.Write(bits, size);
	//m_aac.Encode(bits, size, m_dwINC);
}
void AudioEncode::OnDataAvailable(BYTE* bits, LONG size, DWORD dwFlag, LPVOID lpParameter)
{
	m_resampler.Resample(bits, size);
}

void AudioEncode::OnAudio(BYTE* bits, LONG size, FLOAT ts, LPVOID ps)
{
	m_aac.Encode(bits, size, m_dwINC);
}