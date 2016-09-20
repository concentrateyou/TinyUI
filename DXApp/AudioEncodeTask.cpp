#include "stdafx.h"
#include "AudioEncodeTask.h"


AudioEncodeTask::AudioEncodeTask(MediaCapture* capture)
	:m_capture(capture)
{
}


AudioEncodeTask::~AudioEncodeTask()
{
}

aacEncode*	AudioEncodeTask::GetEncode()
{
	return &m_aac;
}

BOOL AudioEncodeTask::Open(const WAVEFORMATEX& wfx, DWORD dwAudioRate)
{
	BOOL bRes = m_aac.Open(wfx, (DWORD)dwAudioRate);
	if (!bRes)
		return FALSE;
	return TRUE;
}

BOOL AudioEncodeTask::Submit()
{
	Closure s = BindCallback(&AudioEncodeTask::MessagePump, this);
	return TinyTaskBase::Submit(s);
}

void AudioEncodeTask::Exit()
{
	m_signal.SetEvent();
}

void AudioEncodeTask::MessagePump()
{
	for (;;)
	{
		if (m_signal.Lock(0))
		{
			m_aac.Close();
			break;
		}
		m_aac.Encode(m_capture->GetAudioPointer());
	}
}