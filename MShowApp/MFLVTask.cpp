#include "stdafx.h"
#include "MFLVTask.h"

namespace MShow
{
	MFLVTask::MFLVTask()
		:m_sample(0),
		m_bFI(0)
	{

	}


	MFLVTask::~MFLVTask()
	{
	}

	BOOL MFLVTask::Initialize(DX2D& d2d, LPCSTR pzURL)
	{
		m_audioTask.Reset(new MAudioTask(*this, m_clock));
		if (!m_audioTask)
			return FALSE;
		m_audioRenderTask.Reset(new MAudioRenderTask(*m_audioTask, m_clock));
		if (!m_audioRenderTask)
			return FALSE;
		m_videoTask.Reset(new MVideoTask(*this, m_clock));
		if (!m_videoTask)
			return FALSE;
		m_videoRenderTask.Reset(new MVideoRenderTask(*m_videoTask, m_clock, d2d, BindCallback(&MFLVTask::OnVideo, this)));
		if (!m_videoRenderTask)
			return FALSE;
		m_aac.Reset(new AACDecode());
		if (!m_aac)
			return FALSE;
		m_h264.Reset(new H264Decode());
		if (!m_h264)
			return FALSE;
		if (!m_reader.OpenURL(pzURL))
			return FALSE;
		m_script = m_reader.GetScript();
		if (!m_audioRenderTask->Initialize(d2d.GetHWND()))
			return FALSE;
		if (!m_videoRenderTask->Initialize())
			return FALSE;
		return TRUE;
	}

	BOOL MFLVTask::Submit()
	{
		m_close.CreateEvent();
		if (TinyTaskBase::Submit(BindCallback(&MFLVTask::OnMessagePump, this)))
		{
			if (!m_videoTask || !m_videoTask->Submit())
				return FALSE;
			if (!m_videoRenderTask || !m_videoRenderTask->Submit())
				return FALSE;
			if (!m_audioTask || !m_audioTask->Submit())
				return FALSE;
			if (!m_audioRenderTask || !m_audioRenderTask->Submit())
				return FALSE;
			return TRUE;
		}
		return FALSE;
	}

	BOOL MFLVTask::Close(DWORD dwMS)
	{
		if (m_videoTask && m_videoTask->IsValid())
			m_videoTask->Close(dwMS);
		if (m_videoRenderTask && m_videoRenderTask->IsValid())
			m_videoRenderTask->Close(dwMS);
		if (m_audioTask && m_audioTask->IsValid())
			m_audioTask->Close(dwMS);
		if (m_audioRenderTask && m_audioRenderTask->IsValid())
			m_audioRenderTask->Close(dwMS);
		m_close.SetEvent();
		return TinyTaskBase::Close(dwMS);
	}

	MPacketQueue& MFLVTask::GetAudioQueue()
	{
		return m_audioQueue;
	}

	MPacketQueue& MFLVTask::GetVideoQueue()
	{
		return m_videoQueue;
	}

	H264Decode* MFLVTask::GetH264()
	{
		return m_h264;
	}
	AACDecode* MFLVTask::GetAAC()
	{
		return m_aac;
	}

	 FLV_SCRIPTDATA& MFLVTask::GetScript() 
	{
		return m_script;
	}

	void ReleaseBlock(FLV_BLOCK& block)
	{
		SAFE_DELETE_ARRAY(block.audio.data);
		SAFE_DELETE_ARRAY(block.video.data);
	}

	void MFLVTask::OnMessagePump()
	{
		SampleTag tag = { 0 };
		FLV_BLOCK block = { 0 };
		for (;;)
		{
			if (m_close.Lock(0))
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
						if (!m_aac->Open(block.audio.data, block.audio.size, block.audio.bitsPerSample == 0 ? 8 : 16))
						{
							ReleaseBlock(block);
							goto _ERROR;
						}
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
						TinySize size(static_cast<LONG>(m_script.width), static_cast<LONG>(m_script.height));
						if (!m_h264->Initialize(size, size))
						{
							ReleaseBlock(block);
							goto _ERROR;
						}
						if (!m_h264->Open(block.video.data, block.video.size))
						{
							ReleaseBlock(block);
							goto _ERROR;
						}
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

	void MFLVTask::OnVideo(ID2D1Bitmap1* bitmap, INT delay)
	{
		EVENT_VIDEO(bitmap, delay);
	}
}
