#include "stdafx.h"
#include "FLVDecodeTask.h"

namespace FLVPlayer
{
	FLVDecodeTask::FLVDecodeTask()
	{
		m_audioTask.Reset(new FLVAudioTask());
		m_videoTask.Reset(new FLVVideoTask());

		m_onScript.Reset(new Delegate<void(FLV_SCRIPTDATA*)>(this, &FLVDecodeTask::OnScript));
		m_onAudio.Reset(new Delegate<void(BYTE*, LONG, FLV_PACKET*)>(this, &FLVDecodeTask::OnAudio));
		m_onVideo.Reset(new Delegate<void(BYTE*, LONG, FLV_PACKET*)>(this, &FLVDecodeTask::OnVideo));
		m_parse.EVENT_SCRIPT += m_onScript;
		m_parse.EVENT_AUDIO += m_onAudio;
		m_parse.EVENT_VIDEO += m_onVideo;
	}

	FLVDecodeTask::~FLVDecodeTask()
	{
		m_parse.EVENT_SCRIPT -= m_onScript;
		m_parse.EVENT_AUDIO -= m_onAudio;
		m_parse.EVENT_VIDEO -= m_onVideo;
	}
	BOOL FLVDecodeTask::Submit()
	{
		m_wait.CreateEvent();
		return TinyTaskBase::Submit(BindCallback(&FLVDecodeTask::OnMessagePump, this));
	}
	BOOL FLVDecodeTask::Close(DWORD dwMs)
	{
		m_wait.SetEvent();
		m_parse.Close();
		Sleep(800);
		return TRUE;
	}
	void FLVDecodeTask::OnMessagePump()
	{
		m_parse.Open("D:\\test.flv");
		m_parse.Parse();
		m_parse.Close();
	}

	void FLVDecodeTask::OnScript(FLV_SCRIPTDATA* val)
	{
		m_size.cx = static_cast<LONG>(val->width);
		m_size.cy = static_cast<LONG>(val->height);
		m_rate = static_cast<DWORD>(val->framerate);
	}
	void FLVDecodeTask::OnAudio(BYTE* bits, LONG size, FLV_PACKET* packet)
	{
		if (packet->codeID == FLV_CODECID_AAC)
		{
			if (packet->packetType == FLV_AudioSpecificConfig)
			{
				if (!m_audioTask->Submit(bits, size, packet->bitsPerSample))
				{
					m_parse.Close();
				}
			}
			if (packet->packetType == FLV_AACRaw)
			{
				m_lock.Lock();
				//m_packets.push();
				m_lock.Unlock();
			}
		}

	}
	void FLVDecodeTask::OnVideo(BYTE* bits, LONG size, FLV_PACKET* packet)
	{
		if (packet->codeID == FLV_CODECID_H264)
		{
			if (packet->packetType == FLV_AVCDecoderConfigurationRecord)
			{
				if (!m_videoTask->Submit(m_size, m_size, bits, size))
				{
					m_parse.Close();
				}
			}
			if (packet->packetType == FLV_NALU)
			{
				m_lock.Lock();
				AVPacket av;
				av.type = 1;
				av.dts = packet->dts;
				av.pts = packet->pts;
				//m_packets.push(av);
				m_lock.Unlock();
			}
		}
	}
}

