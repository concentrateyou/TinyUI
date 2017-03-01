#include "stdafx.h"
#include "FLVDecodeTask.h"

namespace FLVPlayer
{
	FLVDecodeTask::FLVDecodeTask(HWND hWND)
	{
		m_audioTask.Reset(new FLVAudioTask(this, m_audioLock, hWND));
		m_videoTask.Reset(new FLVVideoTask(this, m_videoLock, hWND));

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
		return TinyTaskBase::Submit(BindCallback(&FLVDecodeTask::OnMessagePump, this));
	}
	BOOL FLVDecodeTask::Close(DWORD dwMs)
	{
		m_parse.Close();
		Sleep(800);
		return TRUE;
	}
	void FLVDecodeTask::OnMessagePump()
	{
		m_parse.Open("D:\\test1.flv");
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
				if (!m_audioTask->Submit(bits, size, packet->bitsPerSample == 0 ? 8 : 16))
				{
					m_parse.Close();
				}
			}
			if (packet->packetType == FLV_AACRaw)
			{
				if (m_audioTask)
				{
					SampleTag av = { 0 };
					av.bits = new BYTE[size];
					memcpy(av.bits, bits, size);
					av.size = size;
					av.sampleDTS = packet->dts;
					av.samplePTS = packet->pts;
					m_audioTask->Push(av);
				}
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
				if (m_videoTask)
				{
					SampleTag av = { 0 };
					av.bits = new BYTE[size];
					memcpy(av.bits, bits, size);
					av.size = size;
					av.sampleDTS = packet->dts;
					av.samplePTS = packet->pts;
					m_videoTask->Push(av);
				}
			}
		}
	}
}

