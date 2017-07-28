#include "stdafx.h"
#include "PublishTask.h"

namespace DXApp
{
	PublishTask::PublishTask(AudioEncode* audioTask, VideoEncode* videoTask)
		:m_audioEncode(audioTask),
		m_videoEncode(videoTask),
		m_baseTime(timeGetTime())
	{
		ASSERT(m_audioEncode || m_videoEncode);
		m_videoDone.Reset(new Delegate<void(BYTE*, LONG, const MediaTag&)>(this, &PublishTask::OnVideoDone));
		m_videoEncode->GetEncode().EVENT_DONE += m_videoDone;
		m_audioDone.Reset(new Delegate<void(BYTE*, LONG, const MediaTag&)>(this, &PublishTask::OnAudioDone));
		m_audioEncode->GetEncode().EVENT_DONE += m_audioDone;
	}


	PublishTask::~PublishTask()
	{

	}

	BOOL PublishTask::Connect(const TinyString& url)
	{
		ASSERT(m_audioEncode || m_videoEncode);
		if (!m_client.Connect(url))
			return FALSE;
		return TRUE;
	}

	BOOL PublishTask::Submit()
	{
		m_close.CreateEvent(FALSE, FALSE, GenerateGUID().c_str(), NULL);
		return TinyTaskBase::Submit(BindCallback(&PublishTask::OnMessagePump, this));
	}
	BOOL PublishTask::Close(DWORD dwMS)
	{
		m_close.SetEvent();
		return TinyTaskBase::Close(dwMS);
	}

	void PublishTask::OnClose()
	{
		ASSERT(m_audioEncode || m_videoEncode);
		m_videoEncode->GetEncode().EVENT_DONE -= m_videoDone;
		m_audioEncode->GetEncode().EVENT_DONE -= m_audioDone;
	}

	void PublishTask::OnVideoDone(BYTE* bits, LONG size, const MediaTag& tag)
	{
		Sample sample;
		memcpy(&sample.mediaTag, &tag, sizeof(tag));
		sample.size = size;
		sample.bits = new BYTE[size];
		memcpy(sample.bits, bits, size);
		sample.mediaTag.dwTime = timeGetTime() - m_baseTime + sample.mediaTag.PTS;
		m_samples.push(sample);
	}

	void PublishTask::OnAudioDone(BYTE* bits, LONG size, const MediaTag& tag)
	{
		Sample sample;
		memcpy(&sample.mediaTag, &tag, sizeof(tag));
		sample.size = size;
		sample.bits = new BYTE[size];
		memcpy(sample.bits, bits, size);
		sample.mediaTag.dwTime = timeGetTime() - m_baseTime + sample.mediaTag.PTS;
		m_samples.push(sample);
	}

	void PublishTask::Publish(Sample& sample)
	{
		switch (sample.mediaTag.dwType)
		{
		case 0://Video
		{
			if (sample.mediaTag.INC == 1)
			{
				WAVEFORMATEX wfx = m_audioEncode->GetParam().GetFormat();
				TinySize size = m_videoEncode->GetSize();
				m_client.SendMetadata(size.cx, size.cy, static_cast<INT>(m_videoEncode->GetFPS()), 1000, wfx, 128);
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
			if (sample.mediaTag.INC == 1)
			{
				vector<BYTE> info;
				m_audioEncode->GetEncode().GetSpecificInfo(info);
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
			if (m_close.Lock(5))
			{
				OnClose();
				break;
			}
			if (!m_samples.empty())
			{
				Sample sample;
				if (m_samples.try_pop(sample))
				{
					Publish(sample);
					SAFE_DELETE_ARRAY(sample.bits);
				}
			}
		}
	}
}