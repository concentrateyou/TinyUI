#include "stdafx.h"
#include "PublishTask.h"


PublishTask::PublishTask(AudioEncode* audioTask, VideoEncode* videoTask)
	:m_audioTask(audioTask),
	m_videoTask(videoTask),
	m_baseTime(timeGetTime())
{
	ASSERT(m_audioTask || m_videoTask);
	m_videoDone.Reset(new Delegate<void(BYTE*, LONG, const MediaTag&)>(this, &PublishTask::OnVideoDone));
	m_videoTask->GetEncode()->EVENT_DONE += m_videoDone;
	m_audioDone.Reset(new Delegate<void(BYTE*, LONG, const MediaTag&)>(this, &PublishTask::OnAudioDone));
	m_audioTask->GetEncode()->EVENT_DONE += m_audioDone;
}


PublishTask::~PublishTask()
{

}

BOOL PublishTask::Connect()
{
	ASSERT(m_audioTask || m_videoTask);
	BOOL bRes = m_client.Connect("rtmp://10.121.86.127/live/test");
	if (!bRes)
		return FALSE;
	return TRUE;
}

BOOL PublishTask::Submit()
{
	m_close.CreateEvent(FALSE, FALSE, GenerateGUID().c_str(), NULL);
	return TinyTaskBase::Submit(std::forward<Closure>(BindCallback(&PublishTask::OnMessagePump, this)));
}
BOOL PublishTask::Close(DWORD dwMS)
{
	m_close.SetEvent();
	return TinyTaskBase::Close(dwMS);
}

void PublishTask::OnClose()
{
	ASSERT(m_audioTask || m_videoTask);
	m_videoTask->GetEncode()->EVENT_DONE -= m_videoDone;
	m_audioTask->GetEncode()->EVENT_DONE -= m_audioDone;
}

void PublishTask::OnVideoDone(BYTE* bits, LONG size, const MediaTag& tag)
{
	Sample sample;
	memcpy(&sample.mediaTag, &tag, sizeof(tag));
	sample.size = size;
	sample.bits = new BYTE[size];
	memcpy(sample.bits, bits, size);
	sample.mediaTag.dwTime = timeGetTime() - m_baseTime + sample.mediaTag.dwPTS;
	m_samples.push(sample);
}

void PublishTask::OnAudioDone(BYTE* bits, LONG size, const MediaTag& tag)
{
	Sample sample;
	memcpy(&sample.mediaTag, &tag, sizeof(tag));
	sample.size = size;
	sample.bits = new BYTE[size];
	memcpy(sample.bits, bits, size);
	sample.mediaTag.dwTime = timeGetTime() - m_baseTime + sample.mediaTag.dwPTS;
	m_samples.push(sample);
}

void PublishTask::Publish(Sample& sample)
{
	switch (sample.mediaTag.dwType)
	{
	case 0://Video
	{
		if (sample.mediaTag.dwINC == 1)
		{
			WAVEFORMATEX wfx = m_audioTask->GetParam()->GetFormat();
			wfx.nSamplesPerSec = 48000;
			wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
			VideoCaptureParam* param = m_videoTask->GetParam();
			m_client.SendMetadata(800, 600, static_cast<INT>(param->GetRate()), 1000, wfx, 128);
		}
		switch (sample.mediaTag.dwFlag)
		{
		case NAL_SPS:
		{
			m_latestSPS.resize(sample.size);
			memcpy(&m_latestSPS[0], sample.bits, sample.size);
		}
		break;
		case NAL_PPS:
		{
			m_latestPPS.resize(sample.size);
			memcpy(&m_latestPPS[0], sample.bits, sample.size);
			m_client.SendSPP(m_latestPPS, m_latestSPS, sample.mediaTag.dwTime);
		}
		break;
		case NAL_SLICE:
		case NAL_SLICE_DPA:
		case NAL_SLICE_DPB:
		case NAL_SLICE_DPC:
		case NAL_SLICE_IDR:
		{
			m_client.SendVideo(sample.bits, sample.size, sample.mediaTag.dwTime);
		}
		break;
		}
	}
	break;
	case 1://Audio
	{
		if (sample.mediaTag.dwINC == 1)
		{
			vector<BYTE> info;
			m_audioTask->GetEncode()->GetSpecificInfo(info);
			m_client.SendAAC(&info[0], info.size());
		}
		m_client.SendAudio(sample.bits, sample.size, sample.mediaTag.dwTime);
	}
	break;
	}
	SAFE_DELETE_ARRAY(sample.bits);
}

void PublishTask::OnMessagePump()
{
	for (;;)
	{
		if (m_close.Lock(0))
		{
			OnClose();
			break;
		}
		if (!m_samples.empty())
		{
			Sleep(10);
			Sample sample;
			if (m_samples.try_pop(sample))
			{
				Publish(sample);
				SAFE_DELETE_ARRAY(sample.bits);
			}
		}
	}
}