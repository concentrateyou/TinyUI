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
		if (!m_readerHLS.Open(pzURL))
			return FALSE;
		m_timer.SetCallback(2000, BindCallback(&MHLSTask::OnSegments, this));
		m_readerTS.SetConfigCallback(BindCallback(&MHLSTask::OnConfigChange, this));
		return TRUE;
	}

	BOOL bF1 = FALSE;
	BOOL bF2 = FALSE;

	void MHLSTask::OnConfigChange(const BYTE* bits, LONG size, BYTE streamType, LPVOID)
	{
		if (streamType == TS_STREAM_TYPE_VIDEO_H264)
		{
			if (!bF1)
			{
				bF1 = TRUE;
				BOOL bRes = FALSE;
				EVENT_AVCDCR(const_cast<BYTE*>(bits), size, bRes);
			}

		}
		if (streamType == TS_STREAM_TYPE_AUDIO_AAC)
		{
			if (!bF2)
			{
				bF2 = TRUE;
				BOOL bRes = FALSE;
				EVENT_ASC(const_cast<BYTE*>(bits), size, 16, bRes);
			}
		}
	}

	void MHLSTask::OnSegments()
	{
		//Ã¿¸ô2ÃëË¢ÐÂ
		m_readerHLS.ReadSegments();
	}

	BOOL MHLSTask::Submit()
	{
		m_bBreak = FALSE;
		m_sample = 0;
		return TinyThread::Submit(BindCallback(&MHLSTask::OnMessagePump, this));
	}

	BOOL MHLSTask::Close(DWORD dwMS)
	{
		m_bBreak = TRUE;
		BOOL bRes = TinyThread::Close(dwMS);
		m_timer.Close();
		m_readerHLS.Close();
		m_readerTS.Close();
		m_sample = 0;
		return bRes;
	}

	TinySize MHLSTask::GetVideoSize()
	{
		//return TinySize(1280, 720);
		return TinySize(480, 288);
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
		HLSReader::Segment segment;
		for (;;)
		{
			if (m_bBreak)
				break;
			ZeroMemory(&segment, sizeof(segment));
			if (!m_readerHLS.GetSegment(segment))
			{
				Sleep(15);
				continue;
			}
			if (m_segment.File == segment.File)
			{
				Sleep(15);
				continue;
			}
			m_segment = segment;
			m_client.Close();
			TRACE("TS OpenURL:%s\n", segment.File.c_str());
			if (!m_client.Open(segment.File.c_str()))
			{
				TRACE("TS OpenURL:%s FAIL\n", segment.File.c_str());
				return FALSE;
			}
			CHAR* ps = NULL;
			INT size = m_client.GetResponse().ReadAsBinary(ps);
			m_readerTS.Close();
			if (!m_readerTS.OpenMemory((BYTE*)(ps), size))
			{
				TRACE("TS OpenMemory FAIL\n");
				break;
			}
			for (;;)
			{
				INT size = m_audioQueue.GetSize() + m_videoQueue.GetSize();
				if (size > MAX_QUEUE_SIZE)
				{
					Sleep(15);
					continue;
				}
				ZeroMemory(&block, sizeof(block));
				if (!m_readerTS.ReadBlock(block))
				{
					ReleaseBlock(block);
					break;
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
		}
		return TRUE;
	}
	MHLSTask::~MHLSTask()
	{
	}
}
