#pragma once
#include "Common.h"
#include "Media/TinyWave.h"
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
#define DEBUG_FFMPEG 1
#if DEBUG_FFMPEG
#pragma comment(lib, "libavutild.lib")
#pragma comment(lib, "libavformatd.lib")
#pragma comment(lib, "libswresampled.lib")
#else
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "swresample.lib")
#endif

using namespace TinyUI::Media;

#define MAX_QUEUE_SIZE				(3 * 1024 * 1024)
#define MIN_FRAMES					25
#define VIDEO_PICTURE_QUEUE_SIZE	3
#define SUBPICTURE_QUEUE_SIZE		16
#define SAMPLE_QUEUE_SIZE			9
#define FRAME_QUEUE_SIZE			FFMAX(SAMPLE_QUEUE_SIZE, FFMAX(VIDEO_PICTURE_QUEUE_SIZE, SUBPICTURE_QUEUE_SIZE))

class FFPacketQueue
{
public:
	FFPacketQueue(TinyConditionVariable& cv);
public:
	BOOL	Push(AVPacket* packet);
	BOOL	Pop(AVPacket* packet, BOOL block);
	void	Flush();
	INT32	GetSize() const;
private:
	INT32					m_serial;
	INT32					m_size;
	INT64					m_duration;
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
	void Uninitialize();
private:
	void OnMessagePump();
	void OnAudioPump();
	void OnVideoPump();
	BOOL OpenAudioStream(AVFormatContext* context);
	BOOL OpenVideoStream(AVFormatContext* context);
	INT DecodeAudio(AVFrame *frame);
	INT DecodeVideo(AVFrame *frame);
private:
	TinyWaveFile			m_waveFile;
	INT						m_indexs[AVMEDIA_TYPE_NB];
	BOOL					m_bEOF;
	INT64					m_startTime;
	INT64					m_duration;
	INT64					m_seek;
	TinyTaskBase			m_readTask;
	TinyTaskBase			m_audioTask;
	TinyTaskBase			m_videoTask;
	AVCodec*				m_audioCodec;
	AVCodec*				m_videoCodec;
	AVCodecContext*			m_audioContext;
	AVCodecContext*			m_videoContext;
	AVFormatContext*		m_context;
	WAVEFORMATEX			m_waveFMT;
	TinyConditionVariable	m_audioCV;
	TinyConditionVariable	m_videoCV;
	TinyConditionVariable	m_readCV;
	FFPacketQueue			m_audioQueue;
	FFPacketQueue			m_videoQueue;
	SwrContext*				m_swr;
};

