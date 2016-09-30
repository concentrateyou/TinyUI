#include "stdafx.h"
#include "AudioEncodeTask.h"


AudioEncodeTask::AudioEncodeTask()
	:m_size(0),
	m_ts(0)
{
}


AudioEncodeTask::~AudioEncodeTask()
{
}

FaacEncode*	AudioEncodeTask::GetEncode()
{
	return &m_aac;
}

AudioCapture* AudioEncodeTask::GetCapture()
{
	return &m_capture;
}

BOOL AudioEncodeTask::Initialize(const AudioCapture::Name& name, const AudioCaptureParam& param)
{
	m_deviceName = name;
	m_audioParam = param;
	m_audioCB = BindCallback(&AudioEncodeTask::OnAudio, this);
	BOOL bRes = m_capture.Initialize(m_deviceName, m_audioCB);
	if (!bRes)
		return FALSE;
	bRes = m_capture.Allocate(param);
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
	m_close.CreateEvent(FALSE, FALSE, GenerateGUID().c_str(), NULL);
	m_capture.Start();
	Closure s = BindCallback(&AudioEncodeTask::OnMessagePump, this);
	return TinyTaskBase::Submit(s);
}

BOOL AudioEncodeTask::Close(DWORD dwMS)
{
	m_close.SetEvent();
	return TinyTaskBase::Close(dwMS);
}

void AudioEncodeTask::OnClose()
{
	m_capture.Uninitialize();
	m_aac.Close();
}

void AudioEncodeTask::OnAudio(BYTE* bits, LONG size, FLOAT ts, LPVOID ps)
{
	if (m_size != size)
	{
		m_size = size;
		m_bits.Reset(new BYTE[m_size]);
		m_queue.Initialize(ROUNDUP_POW_2(m_size * 3));
	}
	m_queue.Write(bits, size);
}

void AudioEncodeTask::OnMessagePump()
{
	for (;;)
	{
		if (m_close.Lock(2))
		{
			OnClose();
			break;
		}
		INT size = m_queue.Read(m_bits, m_size);
		if (size)
		{
			m_aac.Encode(m_bits, size);
		}
	}
}