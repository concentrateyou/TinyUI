#include "stdafx.h"
#include "MFLVTask.h"

namespace MShow
{
	void ReleaseBlock(FLV_BLOCK& block)
	{
		SAFE_DELETE_ARRAY(block.audio.data);
		SAFE_DELETE_ARRAY(block.video.data);
	}
	//////////////////////////////////////////////////////////////////////////
	MFLVTask::MFLVTask(MClock& clock)
		:m_bFI(FALSE),
		m_bClose(FALSE),
		m_sample(0),
		m_clock(clock)
	{
	}

	BOOL MFLVTask::Initialize(LPCSTR pzURL)
	{
		if (!m_reader.OpenURL(pzURL))
			return FALSE;
		m_script = m_reader.GetScript();
		return TRUE;
	}

	BOOL MFLVTask::Submit()
	{
		m_sample = 0;
		m_bFI = FALSE;
		m_bClose = FALSE;
		return TinyTaskBase::Submit(BindCallback(&MFLVTask::OnMessagePump, this));
	}

	BOOL MFLVTask::Close(DWORD dwMS)
	{
		m_bClose = TRUE;
		if (TinyTaskBase::Close(INFINITE))
		{
			ZeroMemory(&m_script, sizeof(m_script));
			m_reader.Close();
			m_bFI = FALSE;
			m_sample = 0;
			m_audioQueue.RemoveAll();
			m_videoQueue.RemoveAll();
			return TRUE;
		}
		return FALSE;
	}

	FLV_SCRIPTDATA&	MFLVTask::GetScript()
	{
		return m_script;
	}

	MPacketQueue& MFLVTask::GetAudioQueue()
	{
		return m_audioQueue;
	}

	MPacketQueue& MFLVTask::GetVideoQueue()
	{
		return m_videoQueue;
	}

	void MFLVTask::OnMessagePump()
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

	MFLVTask::~MFLVTask()
	{
	}
}
