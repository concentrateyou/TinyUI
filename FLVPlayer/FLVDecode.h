#pragma once
#include "Media/TinySoundPlayer.h"
#include "IO/TinyTaskBase.h"
#include "Common/TinyTime.h"
#include "FLVParser.h"
#include "PacketQueue.h"
using namespace Decode;
using namespace TinyUI;
using namespace TinyUI::IO;
using namespace TinyUI::Media;

namespace FLVPlayer
{
#define MAX_QUEUE_SIZE (8 * 1024 * 1024)
#define MAX_AUDIO_QUEUE_SIZE (48 * 1024)
#define MAX_VIDEO_QUEUE_SIZE (6 * 1024 * 1024)

	class FLVDecode;

	class FLVVAudioTask : public TinyTaskBase
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

	class FLVAudioRender : public TinyTaskBase
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
		TinyEvent				m_events[2];
		TinyEvent				m_close;
		TinySoundPlayer			m_player;
		FLVVAudioTask&			m_decode;
		TinyPerformanceTimer	m_timer;
	};

	class FLVVideoTask : public TinyTaskBase
	{
	public:
		FLVVideoTask(FLVDecode& decode);
		~FLVVideoTask();
		BOOL	Submit();
		BOOL	Close(DWORD dwMs) OVERRIDE;
	private:
		void	OnMessagePump();
	public:
		TinyLock	m_lock;
		TinyEvent	m_close;
		PacketQueue	m_queue;
		FLVDecode&	m_decode;
	};

	class FLVVideoRender : public TinyTaskBase
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
		LONGLONG				m_wPTS;
		TinyEvent				m_close;
		FLVVideoTask&			m_decode;
		TinyPerformanceTimer	m_timer;
	};

	class FLVDecode : public TinyTaskBase
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
	private:
		TinyLock					m_lockTime;
		LONG						m_baseTime;
		LONG						m_basePTS;
		HWND						m_hWND;
		TinySize					m_size;
		FLVReader					m_reader;
		TinyLock					m_lock;
		TinyEvent					m_close;
		PacketQueue					m_audioQueue;
		PacketQueue					m_videoQueue;
		TinyScopedPtr<H264Decode>	m_h264;
		TinyScopedPtr<AACDecode>	m_aac;
		FLVAudioRender				m_audioRender;
		FLVVideoRender				m_videoRender;
		FLVVideoTask				m_videoTask;
		FLVVAudioTask				m_audioTask;
	};
}


