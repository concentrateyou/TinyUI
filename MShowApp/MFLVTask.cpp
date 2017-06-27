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
		m_bBreak(FALSE),
		m_sample(0),
		m_clock(clock)
	{
		ZeroMemory(&m_script, sizeof(m_script));
	}

	BOOL MFLVTask::Initialize(LPCSTR pzURL)
	{
		TinyString szName = pzURL;
		if (szName.IndexOf("rtmp://") != -1)
		{
			if (!m_reader.OpenURL(pzURL))
				return FALSE;
		}
		else
		{
			if (!PathFileExists(pzURL))
				return FALSE;
			if (!m_reader.OpenFile(pzURL))
				return FALSE;
		}
		m_script = m_reader.GetScript();
		return TRUE;
	}

	BOOL MFLVTask::Submit()
	{
		m_sample = 0;
		m_bFI = FALSE;
		m_bBreak = FALSE;
		return TinyTaskBase::Submit(BindCallback(&MFLVTask::OnMessagePump, this));
	}

	BOOL MFLVTask::Close(DWORD dwMS)
	{
		m_bBreak = TRUE;
		if (TinyTaskBase::Close(INFINITE))
		{
			ZeroMemory(&m_script, sizeof(m_script));
			m_reader.Close();
			m_bFI = FALSE;
			m_sample = 0;
			ZeroMemory(&m_script, sizeof(m_script));
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
			if (m_bBreak)
				break;
			INT size = m_audioQueue.GetSize() + m_videoQueue.GetSize();
			if (size > MAX_QUEUE_SIZE)
			{
				Sleep(5);
				continue;
			}
			if (!m_reader.ReadBlock(block))
			{
				goto _ERROR;
			}
			if (block.type == FLV_AUDIO)
			{
				if (block.audio.codeID == FLV_CODECID_AAC)
				{
					if (block.audio.packetType == FLV_AudioSpecificConfig)
					{
						BOOL bRes = FALSE;
						EVENT_ASC(block.audio.data, block.audio.size, block.audio.bitsPerSample == 0 ? 8 : 16, bRes);
						if (!bRes)
						{
							goto _ERROR;
						}
					}
					if (block.audio.packetType == FLV_AACRaw)
					{
						if (m_bFI)
						{
							ZeroMemory(&tag, sizeof(tag));
							tag.size = block.video.size;
							tag.bits = new BYTE[tag.size];
							memcpy_s(tag.bits, tag.size, block.audio.data, block.audio.size);
							tag.sampleDTS = block.dts;
							tag.samplePTS = block.pts;
							m_audioQueue.Push(tag);
						}
					}
				}
			}
			if (block.type == FLV_VIDEO)
			{
				if (block.video.codeID == FLV_CODECID_H264)
				{
					if (block.video.packetType == FLV_AVCDecoderConfigurationRecord)
					{
						BOOL bRes = FALSE;
						EVENT_AVCDCR(block.video.data, block.video.size, bRes);
						if (!bRes)
						{
							goto _ERROR;
						}
					}
					if (block.video.packetType == FLV_NALU)
					{
						if (!m_bFI)
						{
							m_bFI = (block.video.codeType == 1);
						}
						if (m_bFI)
						{
							ZeroMemory(&tag, sizeof(tag));
							tag.size = block.video.size;
							tag.bits = new BYTE[tag.size];
							memcpy_s(tag.bits, tag.size, block.video.data, block.video.size);
							tag.sampleDTS = block.dts;
							tag.samplePTS = block.pts;
							m_videoQueue.Push(tag);
						}
					}
				}
			}
			ReleaseBlock(block);
		}
	_ERROR:
		ReleaseBlock(block);
	}

	MFLVTask::~MFLVTask()
	{
	}
}
