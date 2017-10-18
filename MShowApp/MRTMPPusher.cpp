#include "stdafx.h"
#include "MRTMPPusher.h"
#include "MShowApp.h"
#include "MShowController.h"

namespace MShow
{
#define MAX_PUSH_QUEUE_SIZE (1024 * 1024)

	MRTMPPusher::MRTMPPusher()
		:m_bClose(FALSE),
		m_dwReconnect(0)
	{
	}

	MRTMPPusher::~MRTMPPusher()
	{
	}

	BOOL MRTMPPusher::Connect(const TinyString& szURL, DWORD dwRate)
	{
		m_dwRate = dwRate;
		if (m_client.Connect(szURL))
		{
			m_szURL = szURL;
			return TRUE;
		}
		return FALSE;
	}

	BOOL MRTMPPusher::Disconnect()
	{
		return m_client.Disconnect();
	}

	BOOL MRTMPPusher::Submit()
	{
		m_bClose = FALSE;
		return TinyTaskBase::Submit(BindCallback(&MRTMPPusher::OnMessagePump, this));
	}

	BOOL MRTMPPusher::Close(DWORD dwMS)
	{
		m_bClose = TRUE;
		m_dwReconnect = 0;
		if (TinyTaskBase::Close(dwMS))
		{
			m_client.Disconnect();
			return TRUE;
		}
		return FALSE;
	}

	void MRTMPPusher::OnMessagePump()
	{
		MSampleQueue& videoSamples = MShow::MShowApp::GetInstance().GetController().GetVideoEncoder().GetSamples();
		MSampleQueue& audioSamples = MShow::MShowApp::GetInstance().GetController().GetAudioEncoder().GetSamples();
		MAudioEncodeTask& audioTask = MShowApp::GetInstance().GetController().GetAudioEncoder();
		MVideoEncodeTask& videoTask = MShowApp::GetInstance().GetController().GetVideoEncoder();
		//发送媒体信息
		TinySize pulgSize = videoTask.GetSize();
		m_client.SendMetadata(pulgSize.cx, pulgSize.cy, videoTask.GetVideoFPS(), videoTask.GetVideoRate(), audioTask.GetFormat(), audioTask.GetAudioRate());
		Sample videoSample, audioSample;
		for (;;)
		{
			if (m_bClose)
				break;
			if (videoSamples.GetSize() > 0 && audioSamples.GetSize() > 0)
			{
				LONGLONG videoTS = videoSamples.GetMinimumTimeStamp();
				LONGLONG audioTS = audioSamples.GetMinimumTimeStamp();
				if (audioTS > videoTS)
				{
					ZeroMemory(&videoSample, sizeof(videoSample));
					if (videoSamples.Pop(videoSample))
					{
						Publish(&audioTask, &videoTask, videoSample);
					}
				}
				else
				{
					ZeroMemory(&audioSample, sizeof(audioSample));
					if (audioSamples.Pop(audioSample))
					{
						Publish(&audioTask, &videoTask, audioSample);
					}
				}
			}
		}
	}
	void MRTMPPusher::Publish(MAudioEncodeTask* pAudioTask, MVideoEncodeTask* pVideoTask, Sample& sample)
	{
		ASSERT(pAudioTask);
		ASSERT(pVideoTask);
		switch (sample.mediaTag.dwType)
		{
		case 0://Video
		{
			if (sample.mediaTag.INC == 1)
			{
				vector<BYTE>& sps = pVideoTask->GetQSV().GetSPS();
				vector<BYTE>& pps = pVideoTask->GetQSV().GetPPS();
				m_client.SendH264AVC(pps, sps);
			}
			m_client.SendH264NALU(sample.mediaTag.dwFlag, sample.bits, sample.size, sample.mediaTag.dwTime);
		}
		break;
		case 1://Audio
		{
			if (sample.mediaTag.INC == 1)
			{
				vector<BYTE> info;
				pAudioTask->GetAAC().GetSpecificInfo(info);
				m_client.SendAACASC(&info[0], info.size());
			}
			m_client.SendAACRaw(sample.bits, sample.size, sample.mediaTag.dwTime);
		}
		break;
		}
		SAFE_DELETE_ARRAY(sample.bits);
	}
	TinyString MRTMPPusher::GetURL() const
	{
		return m_szURL;
	}
}
