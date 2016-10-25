#include "stdafx.h"
#include "PublishTask.h"


PublishTask::PublishTask(AudioEncode* audioTask, VideoEncode* videoTask)
	:m_audioTask(audioTask),
	m_videoTask(videoTask),
	m_baseTime(timeGetTime())
{
	ASSERT(m_audioTask || m_videoTask);
	m_videoDone.Reset(new Delegate<void(BYTE*, LONG, LONG, DWORD)>(this, &PublishTask::OnVideoDone));
	m_videoTask->GetEncode()->EVENT_DONE += m_videoDone;
	m_audioDone.Reset(new Delegate<void(BYTE*, LONG, LONG, DWORD)>(this, &PublishTask::OnAudioDone));
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
	Closure s = BindCallback(&PublishTask::OnMessagePump, this);
	return TinyTaskBase::Submit(s);
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

void PublishTask::OnVideoDone(BYTE* bits, LONG size, LONG inc, DWORD dwFlag)
{
	Sample sample;
	sample.dwSize = size;
	sample.bits = new BYTE[size];
	memcpy(sample.bits, bits, size);
	sample.dwType = 0;
	sample.dwFlag = dwFlag;
	sample.dwTime = timeGetTime() - m_baseTime;
	sample.dwINC = inc;
	m_lock.Lock();
	m_samples.push(sample);
	m_lock.Unlock();
}

void PublishTask::OnAudioDone(BYTE* bits, LONG size, LONG inc, DWORD dwFlag)
{
	Sample sample;
	sample.dwSize = size;
	sample.bits = new BYTE[size];
	memcpy(sample.bits, bits, size);
	sample.dwType = 1;
	sample.dwFlag = dwFlag;
	sample.dwTime = timeGetTime() - m_baseTime;
	sample.dwINC = inc;
	m_lock.Lock();
	m_samples.push(sample);
	m_lock.Unlock();
}

void PublishTask::OnMessagePump()
{
	for (;;)
	{
		if (m_close.Lock(8))
		{
			OnClose();
			break;
		}
		if (!m_samples.empty())
		{
			m_lock.Lock();
			Sample& sample = m_samples.front();
			switch (sample.dwType)
			{
			case 0://Video
			{
				if (sample.dwINC == 1)
				{
					WAVEFORMATEX wfx = m_audioTask->GetParam()->GetFormat();
					VideoCaptureParam* param = m_videoTask->GetParam();
					m_client.SendMetadataPacket(800, 600, param->GetRate(), 1000, wfx, 128);
				}
				switch (sample.dwFlag)
				{
				case NAL_SPS:
				{
					m_latestSPS.resize(sample.dwSize);
					memcpy(&m_latestSPS[0], sample.bits, sample.dwSize);
				}
				break;
				case NAL_PPS:
				{
					m_latestPPS.resize(sample.dwSize);
					memcpy(&m_latestPPS[0], sample.bits, sample.dwSize);
					m_client.SendSPPacket(m_latestPPS, m_latestSPS, sample.dwTime);
				}
				break;
				case NAL_SLICE:
				case NAL_SLICE_DPA:
				case NAL_SLICE_DPB:
				case NAL_SLICE_DPC:
				case NAL_SLICE_IDR:
				{
					m_client.SendVideoPacket(sample.bits, sample.dwSize, sample.dwTime);
				}
				break;
				}
			}
			break;
			case 1://Audio
			{
				if (sample.dwINC == 1)
				{
					vector<BYTE> info;
					m_audioTask->GetEncode()->GetSpecificInfo(info);
					m_client.SendAACPacket(&info[0], info.size());
				}
				m_client.SendAudioPacket(sample.bits, sample.dwSize, sample.dwTime);
			}
			break;
			}
			SAFE_DELETE_ARRAY(sample.bits);
			m_samples.pop();
			m_lock.Unlock();
		}
	}
}