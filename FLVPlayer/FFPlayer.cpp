#include "stdafx.h"
#include "FFPlayer.h"

FFPacketQueue::FFPacketQueue(TinyConditionVariable& cv, AVPacket& flush)
	:m_cv(cv),
	m_serial(0),
	m_size(0),
	m_duration(0),
	m_flush(flush)
{

}
BOOL FFPacketQueue::Push(AVPacket* packet)
{
	if (!packet)
		return FALSE;
	TinyAutoLock lock(m_lock);
	if (m_list.InsertLast(packet))
	{
		m_size += packet->size + sizeof(*packet);
		m_duration += packet->duration;
		m_cv.Unlock(FALSE);
		return TRUE;
	}
	return FALSE;
}

BOOL FFPacketQueue::Pop(AVPacket* packet, BOOL block)
{
	TinyAutoLock lock(m_lock);
	ITERATOR s = m_list.First();
	if (s != NULL)
	{
		AVPacket* val = m_list.GetAt(s);
		m_list.RemoveAt(s);
		m_size -= val->size + sizeof(*val);
		m_duration -= val->duration;
		*packet = *val;
		av_free(&val);
		return TRUE;
	}
	else
	{
		if (block)
		{
			m_cv.Lock(m_lock, INFINITE);
		}
	}
	return FALSE;
}
void FFPacketQueue::Flush()
{
	ITERATOR s = m_list.First();
	while (s != NULL)
	{
		AVPacket* val = m_list.GetAt(s);
		av_packet_unref(val);
		av_freep(val);
		s = m_list.Next(s);
	}
	m_list.RemoveAll();
}
//////////////////////////////////////////////////////////////////////////
FFPlayer::FFPlayer()
{
}


FFPlayer::~FFPlayer()
{
}

BOOL FFPlayer::Initialize()
{
	av_register_all();
	avformat_network_init();
	m_readTask.Submit(BindCallback(&FFPlayer::OnRead, this));
}

void FFPlayer::OnRead()
{
	const char* szFile = "D:\\1.flv";
	AVDictionaryEntry* entry = NULL;
	AVDictionary **opts = NULL;
	AVInputFormat* input = NULL;
	AVFormatContext *context = NULL;
	input = av_find_input_format(szFile);
	if (!context)
		goto _ERROR;
	context = avformat_alloc_context();
	if (!context)
		goto _ERROR;
	INT iRes = avformat_open_input(&context, szFile, input, NULL);
	if (iRes < 0)
		goto _ERROR;
	for (;;)
	{

	}
_ERROR:
	//TODO
}