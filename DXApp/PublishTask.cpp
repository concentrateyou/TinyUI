#include "stdafx.h"
#include "PublishTask.h"


PublishTask::PublishTask(AudioEncode* audioTask, VideoEncodeTask* videoTask)
	:m_audioTask(audioTask),
	m_videoTask(videoTask)
{
	ASSERT(m_audioTask || m_videoTask);
	m_videoDone.Reset(new Delegate<void(TinyScopedReferencePtr<Sample>&)>(this, &PublishTask::OnVideoDone));
	m_videoTask->GetEncode()->EVENT_DONE += m_videoDone;
	m_audioDone.Reset(new Delegate<void(TinyScopedReferencePtr<Sample>&)>(this, &PublishTask::OnAudioDone));
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

void PublishTask::OnVideoDone(TinyScopedReferencePtr<Sample>& sample)
{
	m_queue.Add(std::move(sample));
}

void PublishTask::OnAudioDone(TinyScopedReferencePtr<Sample>& sample)
{
	m_queue.Add(std::move(sample));
}

void PublishTask::OnMessagePump()
{
	for (;;)
	{
		if (m_close.Lock(30))
		{
			OnClose();
			break;
		}
		if (!m_queue.IsEmpty())
		{
			if (Sample* sample = m_queue.GetSample())
			{
				switch (sample->Track)
				{
				case 0://Video
				{
					if (sample->INC == 1)
					{
						WAVEFORMATEX wfx = m_audioTask->GetParam()->GetFormat();
						VideoCaptureParam* param = m_videoTask->GetParam();
						m_client.SendMetadataPacket(800, 600, param->GetRate(), 1000, wfx, 128);
					}
					switch (sample->Flag)
					{
					case NAL_SPS:
					{
						m_latestSPS.resize(sample->Size);
						memcpy(&m_latestSPS[0], sample->Bits, sample->Size);
					}
					break;
					case NAL_PPS:
					{
						m_latestPPS.resize(sample->Size);
						memcpy(&m_latestPPS[0], sample->Bits, sample->Size);
						m_client.SendSPPacket(m_latestPPS, m_latestSPS, sample->Time);
						TRACE("Video Time:%d\n", sample->Time);
					}
					case NAL_SLICE:
					case NAL_SLICE_DPA:
					case NAL_SLICE_DPB:
					case NAL_SLICE_DPC:
					case NAL_SLICE_IDR:
					{
						m_client.SendVideoPacket(sample->Bits, sample->Size, sample->Time);
						TRACE("Video Time:%d\n", sample->Time);
					}
					break;
					}
				}
				break;
				case 1://Audio
				{
					if (sample->INC == 1)
					{
						vector<BYTE> info;
						m_audioTask->GetEncode()->GetSpecificInfo(info);
						m_client.SendAACPacket(&info[0], info.size());
					}
					m_client.SendAudioPacket(sample->Bits, sample->Size, sample->Time);
					TRACE("Audio Time:%d\n", sample->Time);
				}
				break;
				}
			}
			m_queue.Remove();
			TRACE("size:%d\n", m_queue.GetSize());
		}
	}
}
