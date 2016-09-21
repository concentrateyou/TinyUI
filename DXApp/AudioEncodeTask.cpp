#include "stdafx.h"
#include "AudioEncodeTask.h"


AudioEncodeTask::AudioEncodeTask()
{
}


AudioEncodeTask::~AudioEncodeTask()
{
}

aacEncode*	AudioEncodeTask::GetEncode()
{
	return &m_aac;
}

AudioCapture* AudioEncodeTask::GetCapture()
{
	return &m_audioCapture;
}

BOOL AudioEncodeTask::Initialize(const AudioCapture::Name& name, const AudioCaptureParam& param)
{
	m_deviceName = name;
	m_audioParam = param;
	BOOL bRes = m_audioCapture.Initialize(m_deviceName);
	if (!bRes)
		return FALSE;
	bRes = m_audioCapture.Allocate(param);
	if (!bRes)
		return FALSE;
	
	return TRUE;
}

BOOL AudioEncodeTask::Open(DWORD dwAudioRate)
{
	BOOL bRes = m_aac.Open(m_audioParam.GetFormat(), (DWORD)dwAudioRate);
	if (!bRes)
		return FALSE;
	return TRUE;
}

BOOL AudioEncodeTask::Submit()
{
	m_audioCapture.Start();
	Closure s = BindCallback(&AudioEncodeTask::OnMessagePump, this);
	return TinyTaskBase::Submit(s);
}

void AudioEncodeTask::Exit()
{
	m_signal.SetEvent();
}

void AudioEncodeTask::OnExit()
{
	m_audioCapture.Uninitialize();
	m_aac.Close();
}

void AudioEncodeTask::OnMessagePump()
{
	for (;;)
	{
		if (m_signal.Lock(0))
		{
			OnExit();
			break;
		}
		m_aac.Encode(m_audioCapture.GetPointer());
	}
}