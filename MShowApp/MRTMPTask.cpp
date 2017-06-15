#include "stdafx.h"
#include "MRTMPTask.h"

namespace MShow
{
	void ReleaseBlock(FLV_BLOCK& block)
	{
		SAFE_DELETE_ARRAY(block.audio.data);
		SAFE_DELETE_ARRAY(block.video.data);
	}
	//////////////////////////////////////////////////////////////////////////
	MRTMPTask::MRTMPTask()
		:m_bFI(FALSE),
		m_bClose(FALSE),
		m_sample(0)
	{
	}

	BOOL MRTMPTask::Initialize(LPCSTR pzURL)
	{
		if (!m_reader.OpenURL(pzURL))
			return FALSE;
		m_script = m_reader.GetScript();
	}

	BOOL MRTMPTask::Submit()
	{
		m_bClose = FALSE;
		return TinyTaskBase::Submit(BindCallback(&MRTMPTask::OnMessagePump, this));
	}

	BOOL MRTMPTask::Close()
	{
		m_bClose = TRUE;
		return TinyTaskBase::Close(INFINITE);
	}

	FLV_SCRIPTDATA&	MRTMPTask::GetScript()
	{
		return m_script;
	}

	MPacketQueue& MRTMPTask::GetAudioQueue()
	{
		return m_audioQueue;
	}

	MPacketQueue& MRTMPTask::GetVideoQueue()
	{
		return m_videoQueue;
	}

	void MRTMPTask::OnMessagePump()
	{
		SampleTag tag = { 0 };
		FLV_BLOCK block = { 0 };
		for (;;)
		{
			if (m_bClose)
				break;
			INT size = m_audioQueue.GetSize() + m_videoQueue.GetSize();
			if (size > MAX_QUEUE_SIZE)
			{
				Sleep(15);
				continue;
			}
			if (!m_reader.ReadBlock(block))
			{
				ReleaseBlock(block);
				goto _ERROR;
			}

			if (block.type == FLV_AUDIO)
			{
				if (block.audio.codeID == FLV_CODECID_AAC)
				{
					if (block.audio.packetType == FLV_AudioSpecificConfig)
					{
						EVENT_ASC(block.audio.data, block.audio.size, block.audio.bitsPerSample == 0 ? 8 : 16);
						ReleaseBlock(block);
					}
					if (block.audio.packetType == FLV_AACRaw)
					{
						if (!m_bFI)//等待关键帧
						{
							ReleaseBlock(block);
							continue;
						}
						ZeroMemory(&tag, sizeof(tag));
						tag.bits = block.audio.data;
						tag.size = block.audio.size;
						tag.sample = ++m_sample;
						tag.sampleDTS = block.dts;
						tag.samplePTS = block.pts;
						m_audioQueue.Push(tag);
					}
				}
			}
			if (block.type == FLV_VIDEO)
			{
				if (block.video.codeID == FLV_CODECID_H264)
				{
					if (block.video.packetType == FLV_AVCDecoderConfigurationRecord)
					{
						EVENT_AVCDCR(block.video.data, block.video.size);
						ReleaseBlock(block);
					}
					if (block.video.packetType == FLV_NALU)
					{
						if (!m_bFI)//等待关键帧
						{
							if (block.video.codeType != 1)
							{
								ReleaseBlock(block);
								continue;
							}
							m_bFI = TRUE;
						}
						ZeroMemory(&tag, sizeof(tag));
						tag.sample = ++m_sample;
						tag.bits = block.video.data;
						tag.size = block.video.size;
						tag.sampleDTS = block.dts;
						tag.samplePTS = block.pts;
						m_videoQueue.Push(tag);
					}
				}
			}
		}
	_ERROR:
		m_audioQueue.RemoveAll();
		m_videoQueue.RemoveAll();
	}

	MRTMPTask::~MRTMPTask()
	{
	}
}
