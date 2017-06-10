#include "stdafx.h"
#include "MReadTask.h"

namespace MShow
{
	MReadTask::MReadTask()
		:m_close(FALSE),
		m_index(1)
	{

	}


	MReadTask::~MReadTask()
	{
	}

	BOOL MReadTask::Initialize(LPCSTR pzURL)
	{
		m_reader.Close();
		if (!m_reader.OpenURL(pzURL))
			return FALSE;
		m_script = m_reader.GetScript();
		m_aac.Reset(new AACDecode());
		m_h264.Reset(new H264Decode());
		return TRUE;
	}

	BOOL MReadTask::Close(DWORD dwMS)
	{
		m_close = TRUE;
		return TinyTaskBase::Close(dwMS);
	}

	TinyLock& MReadTask::GetAudioLock()
	{
		return m_locks[0];
	}
	TinyLock& MReadTask::GetVideoLock()
	{
		return m_locks[1];
	}

	MPacketQueue& MReadTask::GetAudioQueue()
	{
		return m_audioQueue;
	}

	MPacketQueue& MReadTask::GetVideoQueue()
	{
		return m_videoQueue;
	}

	H264Decode* MReadTask::GetH264()
	{
		return m_h264;
	}
	AACDecode* MReadTask::GetAAC()
	{
		return m_aac;
	}
	void ReleaseBlock(FLV_BLOCK& block)
	{
		SAFE_DELETE_ARRAY(block.audio.data);
		SAFE_DELETE_ARRAY(block.video.data);
	}

	void MReadTask::OnMessagePump()
	{
		FLV_BLOCK block = { 0 };
		for (;;)
		{
			if (m_close)
				break;
			INT size = m_audioQueue.GetSize() + m_videoQueue.GetSize();
			if (size > MAX_QUEUE_SIZE)
			{
				Sleep(5);
				continue;
			}
			ZeroMemory(&block, sizeof(block));
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
						if (!m_aac->Open(block.audio.data, block.audio.size, block.audio.bitsPerSample == 0 ? 8 : 16))
						{
							goto _ERROR;
						}
					}
					if (block.audio.packetType == FLV_AACRaw)
					{
						if (m_index == 1)
						{
							ReleaseBlock(block);
							continue;
						}
						SampleTag tag = { 0 };
						tag.bits = block.audio.data;
						tag.size = block.audio.size;
						tag.sampleDTS = block.dts;
						tag.samplePTS = block.pts;
						tag.sampleIndex = m_index++;
						m_locks[0].Lock();
						m_audioQueue.Push(tag);
						m_locks[0].Unlock();
					}
				}
			}
			if (block.type == FLV_VIDEO)
			{
				if (block.video.codeID == FLV_CODECID_H264)
				{
					if (block.video.packetType == FLV_AVCDecoderConfigurationRecord)
					{
						TinySize size(static_cast<LONG>(m_script.width), static_cast<LONG>(m_script.height));
						if (!m_h264->Initialize(size, size))
						{
							goto _ERROR;
						}
						if (!m_h264->Open(block.video.data, block.video.size))
						{
							goto _ERROR;
						}
					}
					if (block.video.packetType == FLV_NALU)
					{
						if (m_index == 1 && block.video.codeType != 1)
						{
							ReleaseBlock(block);
							continue;
						}
						SampleTag tag = { 0 };
						tag.bits = block.video.data;
						tag.size = block.video.size;
						tag.sampleDTS = block.dts;
						tag.samplePTS = block.pts;
						tag.sampleIndex = m_index++;
						m_locks[1].Lock();
						m_videoQueue.Push(tag);
						m_locks[1].Unlock();
					}
				}
			}
		}
	_ERROR:
		ReleaseBlock(block);
		m_audioQueue.RemoveAll();
		m_videoQueue.RemoveAll();
	}
}
