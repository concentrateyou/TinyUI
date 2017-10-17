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
		MSampleQueue& videos = MShow::MShowApp::GetInstance().GetController().GetVideoEncoder().GetSamples();
		MSampleQueue& audios = MShow::MShowApp::GetInstance().GetController().GetAudioEncoder().GetSamples();

		MAudioEncodeTask& audio1 = MShowApp::GetInstance().GetController().GetAudioEncoder();
		MVideoEncodeTask& video1 = MShowApp::GetInstance().GetController().GetVideoEncoder();
		TinySize pulgSize = video1.GetSize();
		m_client.SendMetadata(pulgSize.cx, pulgSize.cy, video1.GetVideoFPS(), video1.GetVideoRate(), audio1.GetFormat(), audio1.GetAudioRate());
		Sample video, audio;
		for (;;)
		{
			if (m_bClose)
				break;
			if (videos.GetSize() > 0 && audios.GetSize() > 0)
			{
				LONGLONG videoTS = videos.GetMinimumTimeStamp();
				LONGLONG audioTS = audios.GetMinimumTimeStamp();
				if (audioTS > videoTS)
				{
					ZeroMemory(&video, sizeof(video));
					if (videos.Pop(video))
					{
						TRACE("Video TS: %d\n",video.mediaTag.dwTime);
						Publish(video);
					}
				}
				else
				{
					ZeroMemory(&audio, sizeof(audio));
					if (audios.Pop(audio))
					{
						Publish(audio);
					}
				}
			}
		}
	}
	void MRTMPPusher::Publish(Sample& sample)
	{
		switch (sample.mediaTag.dwType)
		{
		case 0://Video
		{
			if (sample.mediaTag.INC == 1)
			{
				MAudioEncodeTask& audio = MShowApp::GetInstance().GetController().GetAudioEncoder();
				MVideoEncodeTask& video = MShowApp::GetInstance().GetController().GetVideoEncoder();
				vector<BYTE>& sps = video.GetQSV().GetSPS();
				vector<BYTE>& pps = video.GetQSV().GetPPS();
				m_client.SendSPP(pps, sps);
			}
			m_client.SendVideo(sample.mediaTag.dwFlag, sample.bits, sample.size, sample.mediaTag.dwTime);
		}
		break;
		case 1://Audio
		{
			if (sample.mediaTag.INC == 1)
			{
				MAudioEncodeTask& encoder = MShowApp::GetInstance().GetController().GetAudioEncoder();
				vector<BYTE> info;
				encoder.GetAAC().GetSpecificInfo(info);
				m_client.SendAAC(&info[0], info.size());
			}
			m_client.SendAudio(sample.bits, sample.size, sample.mediaTag.dwTime);
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
