#include "stdafx.h"
#include "FFPlayer.h"

FFPacketQueue::FFPacketQueue(TinyConditionVariable& cv)
	:m_cv(cv),
	m_serial(0),
	m_size(0),
	m_duration(0)
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
		s = m_list.Next(s);
	}
	m_list.RemoveAll();
}
INT32 FFPacketQueue::GetSize() const
{
	return m_size;
}
//////////////////////////////////////////////////////////////////////////
FFPlayer::FFPlayer()
	:m_audioContext(NULL),
	m_videoContext(NULL),
	m_audioCodec(NULL),
	m_videoCodec(NULL),
	m_seek(0),
	m_bEOF(FALSE),
	m_audioQueue(m_audioCV),
	m_videoQueue(m_videoCV),
	m_duration(AV_NOPTS_VALUE),
	m_startTime(AV_NOPTS_VALUE)
{
	ZeroMemory(&m_waveFMT, sizeof(m_waveFMT));
}


FFPlayer::~FFPlayer()
{
}

BOOL FFPlayer::Initialize()
{
	av_register_all();
	avdevice_register_all();
	avformat_network_init();
	return m_readTask.Submit(BindCallback(&FFPlayer::OnMessagePump, this));
}

void FFPlayer::Uninitialize()
{
	if (m_context != NULL)
	{
		avformat_close_input(&m_context);
		avformat_free_context(m_context);
		m_context = NULL;
	}
	if (m_audioContext != NULL)
	{
		avcodec_close(m_audioContext);
		avcodec_free_context(&m_audioContext);
		m_audioContext = NULL;
	}
	if (m_videoContext != NULL)
	{
		avcodec_close(m_videoContext);
		avcodec_free_context(&m_videoContext);
		m_videoContext = NULL;
	}
	m_audioQueue.Flush();
	m_videoQueue.Flush();
}

BOOL FFPlayer::OpenVideoStream(AVFormatContext* context)
{
	m_videoContext = avcodec_alloc_context3(NULL);
	if (!m_videoContext)
		return FALSE;
	INT iRes = avcodec_parameters_to_context(m_videoContext, context->streams[AVMEDIA_TYPE_VIDEO]->codecpar);
	if (iRes < 0)
		return FALSE;
	av_codec_set_pkt_timebase(m_videoContext, context->streams[AVMEDIA_TYPE_VIDEO]->time_base);
	m_videoCodec = avcodec_find_decoder(m_videoContext->codec_id);
	if (!m_videoCodec)
		return FALSE;
	m_videoContext->flags2 |= AV_CODEC_FLAG2_FAST;
	if (m_videoCodec->capabilities & AV_CODEC_CAP_DR1)
		m_videoContext->flags |= CODEC_FLAG_EMU_EDGE;
	iRes = avcodec_open2(m_videoContext, m_videoCodec, NULL);
	if (iRes < 0)
		return FALSE;
	return TRUE;
}

BOOL FFPlayer::OpenAudioStream(AVFormatContext* context)
{
	m_audioContext = avcodec_alloc_context3(NULL);
	if (!m_audioContext)
		return FALSE;
	INT iRes = avcodec_parameters_to_context(m_audioContext, context->streams[AVMEDIA_TYPE_AUDIO]->codecpar);
	if (iRes < 0)
		return FALSE;
	av_codec_set_pkt_timebase(m_audioContext, context->streams[AVMEDIA_TYPE_AUDIO]->time_base);
	m_audioCodec = avcodec_find_decoder(m_audioContext->codec_id);
	if (!m_audioCodec)
		return FALSE;
	m_audioContext->flags2 |= AV_CODEC_FLAG2_FAST;
	if (m_audioCodec->capabilities & AV_CODEC_CAP_DR1)
		m_audioContext->flags |= CODEC_FLAG_EMU_EDGE;
	iRes = avcodec_open2(m_audioContext, m_audioCodec, NULL);
	if (iRes < 0)
		return FALSE;
	m_waveFMT.cbSize = 0;
	m_waveFMT.wFormatTag = WAVE_FORMAT_PCM;
	m_waveFMT.nSamplesPerSec = 44100;
	m_waveFMT.nChannels = 2;
	m_waveFMT.nBlockAlign = 4;
	m_waveFMT.wBitsPerSample = 16;
	m_waveFMT.nAvgBytesPerSec = m_waveFMT.nBlockAlign * m_waveFMT.nSamplesPerSec;
	m_waveFile.Create("D:\\123.wav", &m_waveFMT);
	if (m_swr != NULL)
	{
		swr_close(m_swr);
		m_swr = NULL;
	}
	m_swr = swr_alloc_set_opts(m_swr,
		AV_CH_LAYOUT_STEREO,
		AV_SAMPLE_FMT_S16,
		44100,
		m_audioContext->channel_layout,
		m_audioContext->sample_fmt,
		m_audioContext->sample_rate,
		0,
		NULL);
	swr_init(m_swr);
	return m_audioTask.Submit(BindCallback(&FFPlayer::OnAudioPump, this));
}

void FFPlayer::OnMessagePump()
{
	const char* szFile = "D:\\1.flv";
	AVInputFormat*	input = NULL;
	TinyLock		lock;
	AVPacket		packet;
	BOOL			bRange = FALSE;
	INT64			packetTS = 0;
	INT64			streamStartTime = 0;
	av_init_packet(&packet);
	m_context = avformat_alloc_context();
	if (!m_context)
		goto _ERROR;
	av_format_inject_global_side_data(m_context);
	INT iRes = avformat_open_input(&m_context, szFile, input, NULL);
	if (iRes < 0)
		goto _ERROR;
	iRes = avformat_find_stream_info(m_context, NULL);
	if (iRes < 0)
		goto _ERROR;
	memset(m_indexs, -1, sizeof(m_indexs));
	m_indexs[AVMEDIA_TYPE_VIDEO] = av_find_best_stream(m_context, AVMEDIA_TYPE_VIDEO, m_indexs[AVMEDIA_TYPE_VIDEO], -1, NULL, 0);
	m_indexs[AVMEDIA_TYPE_AUDIO] = av_find_best_stream(m_context, AVMEDIA_TYPE_AUDIO, m_indexs[AVMEDIA_TYPE_AUDIO], m_indexs[AVMEDIA_TYPE_VIDEO], NULL, 0);
	//if (m_indexs[AVMEDIA_TYPE_VIDEO] >= 0)
	//{
	//	if (!OpenVideoStream(m_context))
	//		goto _ERROR;
	//}
	if (m_indexs[AVMEDIA_TYPE_AUDIO] >= 0)
	{
		if (!OpenAudioStream(m_context))
			goto _ERROR;
	}

	for (;;)
	{
		/*if ((m_audioQueue.GetSize() + m_videoQueue.GetSize()) > MAX_QUEUE_SIZE)*/
		if ((m_audioQueue.GetSize()) > MAX_QUEUE_SIZE)
		{
			lock.Lock();
			m_readCV.Lock(lock, 10);
			lock.Unlock();
			continue;
		}
		iRes = av_read_frame(m_context, &packet);
		if (iRes < 0)
		{
			if ((iRes == AVERROR_EOF || avio_feof(m_context->pb)) && !m_bEOF)
			{
				if (m_indexs[AVMEDIA_TYPE_VIDEO] >= 0)
				{
					//TODO
				}
				if (m_indexs[AVMEDIA_TYPE_AUDIO] >= 0)
				{
					//TODO
				}
				m_bEOF = TRUE;
			}
			lock.Lock();
			m_readCV.Lock(lock, 10);
			lock.Unlock();
			continue;
		}
		else
		{
			m_bEOF = FALSE;
		}
		streamStartTime = m_context->streams[packet.stream_index]->start_time;
		packetTS = packet.pts == AV_NOPTS_VALUE ? packet.dts : packet.pts;
		bRange = m_duration == AV_NOPTS_VALUE ||
			(packetTS - (streamStartTime != AV_NOPTS_VALUE ? streamStartTime : 0)) *
			av_q2d(m_context->streams[packet.stream_index]->time_base) -
			(double)(m_startTime != AV_NOPTS_VALUE ? m_startTime : 0) / 1000000
			<= ((double)m_duration / 1000000);
		switch (packet.stream_index)
		{
		case AVMEDIA_TYPE_AUDIO:
		{
			if (bRange)
			{
				TRACE("Audio Push\n");
				m_audioQueue.Push(&packet);
			}
		}
		break;
		/*	case AVMEDIA_TYPE_VIDEO:
			{
				if (bRange)
				{
					TRACE("Video Push\n");
					m_videoQueue.Push(&packet);
				}
			}
			break;*/
		}
	}
_ERROR:
	if (m_context != NULL)
	{
		avformat_close_input(&m_context);
		avformat_free_context(m_context);
		m_context = NULL;
	}
	if (m_audioContext != NULL)
	{
		avcodec_close(m_audioContext);
		avcodec_free_context(&m_audioContext);
		m_audioContext = NULL;
	}
	if (m_videoContext != NULL)
	{
		avcodec_close(m_videoContext);
		avcodec_free_context(&m_videoContext);
		m_videoContext = NULL;
	}
}


void FFPlayer::OnAudioPump()
{
	AVFrame *av = av_frame_alloc();
	if (av != NULL)
	{
		if (DecodeAudio(av) < 0)
		{
			//ERROR
		}
		av_frame_free(&av);
		av = NULL;
	}
}

void FFPlayer::OnVideoPump()
{

}

INT FFPlayer::DecodeAudio(AVFrame *av)
{
	INT iRes = AVERROR(EAGAIN);
	AVPacket packet;
	av_init_packet(&packet);
	uint8_t* samples[8];
	samples[0] = NULL;
	for (;;)
	{
		if (m_audioQueue.Pop(&packet, TRUE))
		{
			iRes = avcodec_send_packet(m_audioContext, &packet);
			if (iRes < 0)
			{
				if (iRes == AVERROR(EAGAIN))
				{
					continue;
				}
			}
			else
			{
				iRes = avcodec_receive_frame(m_audioContext, av);
				if (iRes >= 0)
				{
					AVRational tb = { 1, av->sample_rate };
					if (av->pts != AV_NOPTS_VALUE)
					{
						av->pts = av_rescale_q(av->pts, av_codec_get_pkt_timebase(m_audioContext), tb);
					}
					if (samples[0] == NULL)
					{

					}

					TRACE("Audio swr_convert\n");
					swr_convert(m_swr, &samples, av->nb_samples, (const uint8_t**)av->extended_data, av->nb_samples);
					if (samples)
					{
						m_waveFile.Write(samples, av->linesize[0]);
					}
				}
			}
		}
	}
}

INT FFPlayer::DecodeVideo(AVFrame *frame)
{
	INT iRes = AVERROR(EAGAIN);
	for (;;)
	{

	}
}