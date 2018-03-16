#include "stdafx.h"
#include "MHLSTask.h"

namespace HLSPlayer
{
	void ReleaseBlock(TS_BLOCK& block)
	{
		SAFE_DELETE_ARRAY(block.audio.data);
		SAFE_DELETE_ARRAY(block.video.data);
	}
	//////////////////////////////////////////////////////////////////////////
	MHLSTask::MHLSTask(MClock& clock, TinyMsgQueue& queue)
		:m_bBreak(FALSE),
		m_sample(0),
		m_clock(clock),
		m_msgqueue(queue)
	{

	}

	BOOL MHLSTask::Initialize(LPCSTR pzURL, ErrorCallback&& callback)
	{
		if (!PathFileExists(pzURL))
			return FALSE;
		if (!m_reader.OpenFile(pzURL))
			return FALSE;
		m_reader.SetConfigCallback(BindCallback(&MHLSTask::OnConfigChange, this));
		return TRUE;
	}

	void MHLSTask::OnConfigChange(const BYTE* bits, LONG size, BYTE streamType, LPVOID)
	{
		if (streamType == TS_STREAM_TYPE_VIDEO_H264)
		{
			BOOL bRes = FALSE;
			EVENT_AVCDCR(const_cast<BYTE*>(bits), size, bRes);
		}
		if (streamType == TS_STREAM_TYPE_AUDIO_AAC)
		{
			BOOL bRes = FALSE;
			EVENT_ASC(const_cast<BYTE*>(bits), size, 16, bRes);
		}
	}

	BOOL MHLSTask::Submit()
	{
		m_bBreak = FALSE;
		m_sample = 0;
		return TinyTask::Submit(BindCallback(&MHLSTask::OnMessagePump, this));
	}

	BOOL MHLSTask::Close(DWORD dwMS)
	{
		m_bBreak = TRUE;
		BOOL bRes = TinyTask::Close(dwMS);
		m_reader.Close();
		m_sample = 0;
		return bRes;
	}

	TinySize MHLSTask::GetVideoSize()
	{
		return TinySize(1280, 720);
	}

	MPacketQueue& MHLSTask::GetAudioQueue()
	{
		return m_audioQueue;
	}

	MPacketQueue& MHLSTask::GetVideoQueue()
	{
		return m_videoQueue;
	}

	void MHLSTask::OnMessagePump()
	{
		SampleTag tag = { 0 };
		TS_BLOCK block = { 0 };
		Invoke(tag, block);
	}

	BOOL MHLSTask::Invoke(SampleTag& tag, TS_BLOCK& block)
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
			ZeroMemory(&block, sizeof(block));
			if (!m_reader.ReadBlock(block))
			{
				ReleaseBlock(block);
				return FALSE;
			}
			if (block.streamType == TS_STREAM_TYPE_AUDIO_AAC)
			{
				ZeroMemory(&tag, sizeof(tag));
				tag.size = block.audio.size;
				tag.bits = block.audio.data;
				tag.sampleDTS = block.dts;
				tag.samplePTS = block.pts;
				m_audioQueue.Push(tag);
			}
			if (block.streamType == TS_STREAM_TYPE_VIDEO_H264)
			{
				ZeroMemory(&tag, sizeof(tag));
				tag.size = block.video.size;
				tag.bits = block.video.data;
				tag.sampleDTS = block.dts;
				tag.samplePTS = block.pts;
				m_videoQueue.Push(tag);
			}
		}
		return TRUE;
	}
	MHLSTask::~MHLSTask()
	{
	}
}
