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
	WAVEFORMATEX s = m_audioParam.GetFormat();
	BOOL bRes = m_aac.Open(s, (DWORD)dwAudioRate);
	if (!bRes)
		return FALSE;
	bRes = m_wasCapture.Open();
	if (!bRes)
		return FALSE;
	m_resample16CB = BindCallback(&AudioEncode::OnResampleDataAvailable16, this);
	m_resampler.Open(m_wasCapture.GetInputFormat(), &s, m_resample16CB);
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
	m_wasCapture.Close();
	return TRUE;
}

void AudioEncode::OnResampleDataAvailable16(BYTE* bits, LONG size, LPVOID lpParameter)
{
	if (m_buffer.m_size >= 2048)
	{
		m_aac.Encode(m_buffer.m_value, m_buffer.m_size, m_dwINC);
		m_buffer.Remove(0, m_buffer.m_size);
	}
	m_buffer.Add(bits, size);
}
void AudioEncode::OnDataAvailable(BYTE* bits, LONG size, LPVOID lpParameter)
{
	m_resampler.Resample(bits, size * 8);
}

void AudioEncode::OnAudio(BYTE* bits, LONG size, FLOAT ts, LPVOID ps)
{
}