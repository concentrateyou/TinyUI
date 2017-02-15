#include "stdafx.h"
#include "FLVDecodeTask.h"

namespace FLVPlayer
{
	FLVDecodeTask::FLVDecodeTask()
	{
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
				m_aac.Reset(new AACDecode());
				m_aac->Initialize(BindCallback(&FLVDecodeTask::OnAAC, this));
				if (!m_aac->Open(bits, size, packet->bitsPerSample))
				{
					m_parse.Close();
				}
			}
			if (packet->packetType == FLV_AACRaw)
			{
				ASSERT(m_aac);
				m_aac->Decode(bits, size);
			}
		}

	}
	void FLVDecodeTask::OnVideo(BYTE* bits, LONG size, FLV_PACKET* packet)
	{
		if (packet->codeID == FLV_CODECID_H264)
		{
			if (packet->packetType == FLV_AVCDecoderConfigurationRecord)
			{
				ASSERT(!m_size.IsEmpty());
				m_h264.Reset(new H264Decode());
				m_h264->Initialize(m_size, m_size, BindCallback(&FLVDecodeTask::OnH264, this));
				if (!m_h264->Open(bits, size))
				{
					m_parse.Close();
				}
			}
			if (packet->packetType == FLV_NALU)
			{
				ASSERT(m_h264);
				m_h264->Decode(bits, size);
			}
		}
	}
	void FLVDecodeTask::OnH264(BYTE* bits, LONG size, LPVOID ps)
	{

	}
	void FLVDecodeTask::OnAAC(BYTE* bits, LONG size, LPVOID ps)
	{
		if (bits == NULL && size == 0)
		{
			m_waveFMT = *reinterpret_cast<WAVEFORMATEX*>(ps);
		}
		else
		{

		}
	}
}

