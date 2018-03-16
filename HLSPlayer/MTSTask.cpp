#include "stdafx.h"
#include "MTSTask.h"

namespace HLSPlayer
{
	void ReleaseBlock(TS_BLOCK& block)
	{
		SAFE_DELETE_ARRAY(block.audio.data);
		SAFE_DELETE_ARRAY(block.video.data);
	}
	//////////////////////////////////////////////////////////////////////////
	MTSTask::MTSTask(MClock& clock, TinyMsgQueue& queue)
		:m_bBreak(FALSE),
		m_sample(0),
		m_clock(clock),
		m_msgqueue(queue)
	{

	}

	BOOL MTSTask::Initialize(LPCSTR pzURL, ErrorCallback&& callback)
	{
		if (!PathFileExists(pzURL))
			return FALSE;
		if (!m_reader.OpenFile(pzURL))
			return FALSE;
		m_reader.SetConfigCallback(BindCallback(&MTSTask::OnConfigChange, this));
		return TRUE;
	}

	void MTSTask::OnConfigChange(const BYTE* bits, LONG size, BYTE streamType, LPVOID)
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

	BOOL MTSTask::Submit()
	{
		m_bBreak = FALSE;
		m_sample = 0;
		return TinyTask::Submit(BindCallback(&MTSTask::OnMessagePump, this));
	}

	BOOL MTSTask::Close(DWORD dwMS)
	{
		m_bBreak = TRUE;
		BOOL bRes = TinyTask::Close(dwMS);
		m_reader.Close();
		m_sample = 0;
		return bRes;
	}

	TinySize MTSTask::GetVideoSize()
	{
		return TinySize(1280, 720);
	}

	MPacketQueue& MTSTask::GetAudioQueue()
	{
		return m_audioQueue;
	}

	MPacketQueue& MTSTask::GetVideoQueue()
	{
		return m_videoQueue;
	}

	void MTSTask::OnMessagePump()
	{
		SampleTag tag = { 0 };
		TS_BLOCK block = { 0 };
		Invoke(tag, block);
	}

	BOOL MTSTask::Invoke(SampleTag& tag, TS_BLOCK& block)
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
	MTSTask::~MTSTask()
	{
	}
}
