#include "stdafx.h"
#include "MRTMPPusher.h"
#include "MShowApp.h"
#include "MShowController.h"

namespace MShow
{
#define MAX_PUSH_QUEUE_SIZE (1024 * 1024)

	MRTMPPusher::MRTMPPusher()
		:m_bClose(FALSE),
		m_bAVSC(FALSE),
		m_dwReconnect(0)
	{
	}

	MRTMPPusher::~MRTMPPusher()
	{
	}

	BOOL MRTMPPusher::Connect(const TinyString& szURL, DWORD dwRate)
	{
		m_dwRate = dwRate;
		if (m_pusher.Connect(szURL))
		{
			m_szURL = szURL;
			return TRUE;
		}
		return FALSE;
	}

	BOOL MRTMPPusher::Disconnect()
	{
		return m_pusher.Disconnect();
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
			m_pusher.Disconnect();
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
		m_pusher.SendScript(pulgSize.cx, pulgSize.cy, videoTask.GetVideoFPS(), videoTask.GetVideoRate(), audioTask.GetFormat(), audioTask.GetAudioRate());
		Sample videoSample, audioSample;
		for (;;)
		{
			if (m_bClose)
				break;
			if (videoSamples.GetSize() > 0 && audioSamples.GetSize() > 0)
			{
				if (!m_bAVSC)
				{
					vector<BYTE> info;
					audioTask.GetAAC().GetSpecificInfo(info);
					m_pusher.SendAACASC(&info[0], info.size());
					vector<BYTE>& sps = videoTask.GetQSV().GetSPS();
					vector<BYTE>& pps = videoTask.GetQSV().GetPPS();
					m_pusher.SendH264AVC(pps, sps);
					m_bAVSC = TRUE;
				}
				//TRACE("videoSamples:%d, audioSamples:%d\n", videoSamples.GetCount(), audioSamples.GetCount());
				LONGLONG videoTS = videoSamples.GetMinimumTimeStamp();
				LONGLONG audioTS = audioSamples.GetMinimumTimeStamp();
				if (audioTS > videoTS)
				{
					ZeroMemory(&videoSample, sizeof(videoSample));
					if (videoSamples.Pop(videoSample))
					{
						m_pusher.SendH264NALU(videoSample.mediaTag.dwFlag, videoSample.bits, videoSample.size, videoSample.mediaTag.dwTime);
					}
					SAFE_DELETE_ARRAY(videoSample.bits);

				}
				else
				{
					ZeroMemory(&audioSample, sizeof(audioSample));
					if (audioSamples.Pop(audioSample))
					{
						m_pusher.SendAACRaw(audioSample.bits, audioSample.size, audioSample.mediaTag.dwTime);
					}
					SAFE_DELETE_ARRAY(audioSample.bits);

				}
			}
		}
	}
	TinyString MRTMPPusher::GetURL() const
	{
		return m_szURL;
	}
}
