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
	MFLVTask::MFLVTask(MClock& clock, TinyMsgQueue& queue)
		:m_bFI(FALSE),
		m_bBreak(FALSE),
		m_sample(0),
		m_clock(clock),
		m_msgqueue(queue)
	{
		ZeroMemory(&m_script, sizeof(m_script));
	}

	BOOL MFLVTask::Initialize(LPCSTR pzURL, ErrorCallback&& callback)
	{
		TinyString szName = pzURL;
		if (szName.IndexOf("rtmp://") != -1 || szName.IndexOf("http://") != -1)
		{
			if (!m_reader.OpenURL(pzURL, std::move(callback)))
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
		m_bBreak = FALSE;
		m_bFI = FALSE;
		m_sample = 0;
		return TinyTask::Submit(BindCallback(&MFLVTask::OnMessagePump, this));
	}

	BOOL MFLVTask::Close(DWORD dwMS)
	{
		m_bBreak = TRUE;
		BOOL bRes = TinyTask::Close(dwMS);
		m_reader.Close();
		m_bFI = FALSE;
		m_sample = 0;
		ZeroMemory(&m_script, sizeof(m_script));
		return bRes;
	}

	FLV_SCRIPTDATA&	MFLVTask::GetScript()
	{
		return m_script;
	}

	LONGLONG MFLVTask::GetBasePTS()
	{
		return m_reader.GetBasePTS();
	}

	TinySize MFLVTask::GetVideoSize()
	{
		FLV_SCRIPTDATA s = m_reader.GetScript();
		TinySize size;
		size.cx = static_cast<LONG>(s.width);
		size.cy = static_cast<LONG>(s.height);
		return size;
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
		Invoke(tag, block);
	}

	BOOL MFLVTask::Invoke(SampleTag& tag, FLV_BLOCK& block)
	{
		for (;;)
		{
			if (m_bBreak)
				break;
			INT size = m_audioQueue.GetSize() + m_videoQueue.GetSize();
			if (size > MAX_QUEUE_SIZE)
			{
				Sleep(15);
				continue;
			}
			//¶ÁÈ¡Ò»¸öTag
			ZeroMemory(&block, sizeof(block));
			if (!m_reader.ReadBlock(block))
			{
				TRACE("ReadBlock FAIL\n");
				LOG(ERROR) << "ReadBlock FAIL";
				ReleaseBlock(block);
				MSG msg = { 0 };
				msg.message = WM_FLV_PARSE_FAIL;
				m_msgqueue.PostMsg(msg);
				return FALSE;
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
							TRACE("ReadBlock FAIL\n");
							LOG(ERROR) << "ReadBlock FAIL";
							ReleaseBlock(block);
							MSG msg = { 0 };
							msg.message = WM_FLV_PARSE_FAIL;
							m_msgqueue.PostMsg(msg);
							return FALSE;
						}
						ReleaseBlock(block);
					}
					if (block.audio.packetType == FLV_AACRaw)
					{
						if (!m_bFI)
						{
							ReleaseBlock(block);
						}
						else
						{
							ZeroMemory(&tag, sizeof(tag));
							tag.size = block.audio.size;
							tag.bits = block.audio.data;
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
							TRACE("ReadBlock FAIL\n");
							LOG(ERROR) << "ReadBlock FAIL";
							ReleaseBlock(block);
							MSG msg = { 0 };
							msg.message = WM_FLV_PARSE_FAIL;
							m_msgqueue.PostMsg(msg);
							return FALSE;
						}
						ReleaseBlock(block);
					}
					if (block.video.packetType == FLV_NALU)
					{
						if (!m_bFI)
						{
							m_bFI = (block.video.codeType == 1);
						}
						if (!m_bFI)
						{
							ReleaseBlock(block);
						}
						else
						{
							ZeroMemory(&tag, sizeof(tag));
							tag.size = block.video.size;
							tag.bits = block.video.data;
							tag.sampleDTS = block.dts;
							tag.samplePTS = block.pts;
							m_videoQueue.Push(tag);
						}
					}
				}
			}
		}
		return TRUE;
	}
	MFLVTask::~MFLVTask()
	{
	}
}
