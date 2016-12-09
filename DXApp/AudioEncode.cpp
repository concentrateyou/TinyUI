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
	BOOL bRes = m_capture.Initialize(m_name, BindCallback(&AudioEncode::OnAudio, this));
	if (!bRes)
		return FALSE;
	bRes = m_capture.Allocate(param);
	if (!bRes)
		return FALSE;
	return TRUE;
}

BOOL AudioEncode::Open(DWORD dwAudioRate)
{
	WAVEFORMATEX s = m_audioParam.GetFormat();
	BOOL bRes = m_aac.Open(s, (DWORD)dwAudioRate);
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

void AudioEncode::OnAudio(BYTE* bits, LONG size, FLOAT ts, LPVOID ps)
{
	m_aac.Encode(bits, size, m_dwINC);
}