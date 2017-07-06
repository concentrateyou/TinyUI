#pragma once
#include "Common.h"
extern "C"
{
#include "libavutil/avstring.h"
#include "libavutil/eval.h"
#include "libavutil/mathematics.h"
#include "libavutil/pixdesc.h"
#include "libavutil/imgutils.h"
#include "libavutil/dict.h"
#include "libavutil/parseutils.h"
#include "libavutil/samplefmt.h"
#include "libavutil/avassert.h"
#include "libavutil/time.h"
#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include "libswscale/swscale.h"
#include "libavutil/opt.h"
#include "libavcodec/avfft.h"
#include "libswresample/swresample.h"
}


class FFPacketQueue
{
public:
	FFPacketQueue(TinyConditionVariable& cv,AVPacket& flush);
public:
	BOOL Push(AVPacket* packet);
	BOOL Pop(AVPacket* packet, BOOL block);
	void Flush();
private:
	INT32					m_serial;
	INT32					m_size;
	INT64					m_duration;
	AVPacket&				m_flush;
	TinyLock				m_lock;
	TinyConditionVariable&	m_cv;
	TinyLinkList<AVPacket*>	m_list;
};

class FFPlayer
{
public:
	FFPlayer();
	~FFPlayer();
	BOOL Initialize();
private:
	void OnRead();
private:
	TinyTaskBase	m_readTask;
};

