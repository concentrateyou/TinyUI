#pragma once
#include "Media/TinySoundPlayer.h"
#include "IO/TinyThread.h"
#include "Common/TinyTime.h"
#include "FLVReader.h"
#include "FLVWriter.h"
#include "PacketQueue.h"
#include "QSVDecoder.h"
#include "DX9Graphics2D.h"
#include "DX9Image2D.h"
#include "DX9RenderView.h"
#include "Media/TinyMFIntelQSVDecode.h"
using namespace Decode;
using namespace TinyUI;
using namespace TinyUI::IO;
using namespace TinyUI::Media;
using namespace DXFramework;

namespace FLVPlayer
{
#define MAX_QUEUE_SIZE (1 * 1024 * 1024)
#define MAX_AUDIO_QUEUE_SIZE (48 * 1024)
#define MAX_VIDEO_QUEUE_SIZE (6 * 1024 * 1024)

	class FLVDecode;

	class FLVVAudioTask : public TinyThread
	{
	public:
		FLVVAudioTask(FLVDecode& decode);
		~FLVVAudioTask();
		BOOL	Submit();
		BOOL	Close(DWORD dwMs) OVERRIDE;
	private:
		void	OnMessagePump();
	public:
		TinyEvent	m_close;
		TinyLock	m_lock;
		PacketQueue	m_queue;
		FLVDecode&	m_decode;
	};

	class FLVAudioRender : public TinyThread
	{
	public:
		FLVAudioRender(FLVVAudioTask& decode);
		~FLVAudioRender();
		BOOL	Submit();
		BOOL	Close(DWORD dwMs) OVERRIDE;
	private:
		void	OnMessagePump();
	private:
		DWORD					m_dwMS;
		LONGLONG				m_pts;
		BOOL					m_bFlag;
		BOOL					m_bInitialize;
		TinyEvent				m_events[3];
		TinyEvent				m_close;
		TinySoundPlayer			m_player;
		FLVVAudioTask&			m_decode;
	};

	class FLVVideoTask : public TinyThread
	{
	public:
		FLVVideoTask(FLVDecode& decode);
		~FLVVideoTask();
		BOOL	Submit();
		BOOL	Close(DWORD dwMs) OVERRIDE;
	private:
		void	OnMessagePump();
		void	OnQSV(Media::SampleTag& tag);
	public:
		TinyLock	m_lock;
		TinyEvent	m_close;
		PacketQueue	m_queue;
		FLVDecode&	m_decode;
	};

	class FLVVideoRender : public TinyThread
	{
	public:
		FLVVideoRender(FLVVideoTask& decode);
		~FLVVideoRender();
		BOOL	Submit();
		BOOL	Close(DWORD dwMs) OVERRIDE;
	private:
		void	OnMessagePump();
		void	OnRender(BYTE* bits, LONG size);
	private:
		DWORD					m_dwMS;
		BOOL					m_bFlag;
		BOOL					m_bBreak;
		LONGLONG				m_wPTS;
		FLVVideoTask&			m_decode;
		TinyPerformanceTime		m_time;
		DX9Graphics2D			m_graphics;
		DX9Image2D				m_image;
		DX9Font2D				m_font2D;
	};

	class FLVDecode : public TinyThread
	{
		friend class FLVAudioRender;
		friend class FLVVideoRender;
		friend class FLVVideoTask;
		friend class FLVVAudioTask;
	public:
		FLVDecode(HWND hWND);
		~FLVDecode();
		BOOL	Submit();
		BOOL	Close(DWORD dwMs) OVERRIDE;
	private:
		void	OnMessagePump();
		void	OnData(BYTE* bits, LONG size, LPVOID ps);
		void	OnQSV(BYTE* bits, LONG size, INT pts);
		void	OnError(INT iError);
	private:
		BOOL						m_bFirstI;
		LONG						m_currentAudioPTS;
		LONG						m_basePTS;
		HWND						m_hWND;
		TinySize					m_size;
		FLVReader					m_reader;
		FLVWriter					m_writer;
		TinyLock					m_lock1;
		TinyLock					m_lock2;
		TinyEvent					m_close;
		TinyEvent					m_audioEvent;
		PacketQueue					m_audioQueue;
		PacketQueue					m_videoQueue;
		TinyScopedPtr<x264Decoder>	m_x264;
		TinyScopedPtr<AACDecoder>	m_aac;
		FLVAudioRender				m_audioRender;
		FLVVideoRender				m_videoRender;
		FLVVideoTask				m_videoTask;
		FLVVAudioTask				m_audioTask;
	};
}


