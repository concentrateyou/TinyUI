#include "stdafx.h"
#include "AudioEncode.h"

AudioEncode::AudioEncode()
	:m_dwOffset(0)
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
	/*BOOL bRes = m_capture.Initialize(m_name, m_audioCB);
	if (!bRes)
		return FALSE;
	bRes = m_capture.Allocate(param);
	if (!bRes)
		return FALSE;*/
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
	/*m_resample16CBF = BindCallback(&AudioEncode::OnResampleDataAvailable16F, this);
	m_resampler.Open(m_wasCapture.GetInputFormat(), &s, m_resample16CBF);*/
	/*bRes = m_capture.Start();
	if (!bRes)
		return FALSE;*/
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
	m_resampler.Close();
	return TRUE;
}

void AudioEncode::OnResampleDataAvailable16(BYTE* bits, LONG count, LPVOID lpParameter)
{
	if (count <= 0)
		return;
	if ((m_dwOffset + count * 4) >= 4096)
	{
		DWORD dwL = 4096 - m_dwOffset;
		memcpy(m_buffer + m_dwOffset, bits, dwL);
		m_aac.Encode(m_buffer, 4096, m_dwINC);
		memset(m_buffer, 0, 4096);
		m_dwOffset = count * 4 - dwL;
		memcpy(m_buffer, bits, m_dwOffset);
	}
	else
	{
		memcpy(m_buffer + m_dwOffset, bits, count * 4);
		m_dwOffset += (count * 4);
	}
}

void AudioEncode::OnResampleDataAvailable16F(FLOAT* bits, LONG count, LPVOID lpParameter)
{
	if (count <= 0)
		return;
	count = count / 4;
	if ((m_dwOffset + count * 4) >= 4096)
	{
		DWORD dwL = 4096 - m_dwOffset;
		memcpy(m_buffer + m_dwOffset, bits, dwL);
		m_aac.Encode(m_buffer, 4096, m_dwINC);
		memset(m_buffer, 0, 4096);
		m_dwOffset = count * 4 - dwL;
		memcpy(m_buffer, bits, m_dwOffset);
	}
	else
	{
		memcpy(m_buffer + m_dwOffset, bits, count * 4);
		m_dwOffset += (count * 4);
	}
}

void AudioEncode::OnDataAvailable(BYTE* bits, LONG count, LPVOID lpParameter)
{
	m_resampler.Resample(bits, count);
}

void AudioEncode::OnAudio(BYTE* bits, LONG size, FLOAT ts, LPVOID ps)
{
}