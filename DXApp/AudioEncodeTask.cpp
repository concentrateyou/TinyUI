#include "stdafx.h"
#include "AudioEncodeTask.h"


AudioEncodeTask::AudioEncodeTask()
	:m_size(0),
	m_ts(0),
	m_bFull(FALSE)
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

AudioCaptureParam*	AudioEncodeTask::GetParam()
{
	return &m_audioParam;
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
	//»º³å0.5s
	if (m_ts <= 500)
	{
		TinyScopedReferencePtr<RawSample> sample(new RawSample(size));
		sample->SampleTime = ts;
		sample->Time = GetCurrentTime();
		sample->Fill(bits, size);
		m_queue.Add(sample);
		m_ts += ts;
		m_bFull = FALSE;
	}
	else
	{
		m_bFull = TRUE;
	}
}

void AudioEncodeTask::OnMessagePump()
{
	DWORD dwTime = 0;
	for (;;)
	{
		DWORD s = 1000 / 30;
		s = dwTime > s ? 0 : s - dwTime;
		if (m_close.Lock(s))
		{
			OnClose();
			break;
		}
		m_timer.BeginTime();
		if (m_bFull && !m_queue.IsEmpty())
		{
			if (RawSample* sample = m_queue.GetSample())
			{
				m_aac.Encode(sample->Bits, sample->Size, sample->Time);
				m_ts -= sample->SampleTime;
			}
			m_queue.Remove();
		}
		m_timer.EndTime();
		dwTime = m_timer.GetMicroseconds() / 1000;
	}
}