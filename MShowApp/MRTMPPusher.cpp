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
		for (;;)
		{
			if (m_bClose)
				break;
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
	void MRTMPPusher::Publish(Sample& sample)
	{
		switch (sample.mediaTag.dwType)
		{
		case 0://Video
		{
			if (sample.mediaTag.INC == 1)
			{
				MAudioEncodeTask& audio = MShowApp::Instance().GetController().GetAudioEncoder();
				MVideoEncodeTask& video = MShowApp::Instance().GetController().GetVideoEncoder();
				TinySize pulgSize = video.GetSize();
				m_client.SendMetadata(pulgSize.cx, pulgSize.cy, video.GetVideoFPS(), video.GetVideoRate(), audio.GetFormat(), audio.GetAudioRate());
				vector<BYTE>& sps = video.GetQSV().GetSPS();
				vector<BYTE>& pps = video.GetQSV().GetPPS();
				m_client.SendSPP(pps, sps, sample.mediaTag.dwTime);
			}
			m_client.SendVideo(sample.mediaTag.dwFlag, sample.bits, sample.size, sample.mediaTag.dwTime);
		}
		break;
		case 1://Audio
		{
			if (sample.mediaTag.INC == 1)
			{
				MAudioEncodeTask& encoder = MShowApp::Instance().GetController().GetAudioEncoder();
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
}
