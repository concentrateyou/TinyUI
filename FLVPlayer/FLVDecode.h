#pragma once
#include "FLVParser.h"
#include "FLVPlayer.h"
#include "IO/TinyTaskBase.h"
#include "FLVAudio.h"
#include "FLVVideo.h"
#include "PacketQueue.h"
using namespace Decode;
using namespace TinyUI;
using namespace TinyUI::IO;

namespace FLVPlayer
{
#define MAX_QUEUE_SIZE (8 * 1024 * 1024)
#define MAX_AUDIO_QUEUE_SIZE (48 * 1024)

	class FLVDecode;

	class FLVVAudioDecodeTask : public TinyTaskBase
	{
	public:
		FLVVAudioDecodeTask(FLVDecode& decode);
		~FLVVAudioDecodeTask();
		BOOL	Submit();
		BOOL	Close(DWORD dwMs) OVERRIDE;
	private:
		void	OnMessagePump();
	public:
		TinyLock	m_lock;
		PacketQueue	m_queue;
		FLVDecode&	m_decode;
	};


	class FLVAudioRender : public TinyTaskBase
	{
	public:
		FLVAudioRender(FLVVAudioDecodeTask& decode);
		~FLVAudioRender();
		BOOL	Submit();
		BOOL	Close(DWORD dwMs) OVERRIDE;
	private:
		void	OnMessagePump();
	private:
		DWORD			m_dwMS;
		LONGLONG		m_pts;
		BOOL			m_bFlag;
		BOOL			m_bInitialize;
		TinyEvent		m_events[2];
		TinySoundPlayer	m_player;
		FLVVAudioDecodeTask&	m_decode;
	};

	class FLVVideoDecodeTask : public TinyTaskBase
	{
	public:
		FLVVideoDecodeTask(FLVDecode& decode);
		~FLVVideoDecodeTask();
		BOOL	Submit();
		BOOL	Close(DWORD dwMs) OVERRIDE;
	private:
		void	OnMessagePump();
	public:
		TinyLock	m_lock;
		PacketQueue	m_queue;
		FLVDecode&	m_decode;
	};

	
	class FLVVideoRender : public TinyTaskBase
	{
	public:
		FLVVideoRender(FLVVideoDecodeTask& decode);
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
		FLVVideoDecodeTask&			m_decode;
		TinyPerformanceTimer	m_timer;
	};

	class FLVDecode : public TinyTaskBase
	{
		friend class FLVAudioRender;
		friend class FLVVideoRender;
		friend class FLVVideoDecodeTask;
		friend class FLVVAudioDecodeTask;
	public:
		FLVDecode(HWND hWND);
		~FLVDecode();
		BOOL	Submit();
		BOOL	Close(DWORD dwMs) OVERRIDE;
	private:
		void	OnMessagePump();
	private:
		DWORD						m_dwBaseMS;
		HWND						m_hWND;
		TinySize					m_size;
		FLVReader					m_reader;
		TinyLock					m_lock;
		PacketQueue					m_audioQueue;
		PacketQueue					m_videoQueue;
		TinyScopedPtr<H264Decode>	m_h264;
		TinyScopedPtr<AACDecode>	m_aac;
		FLVAudioRender				m_audioRender;
		FLVVideoRender				m_videoRender;
		FLVVideoDecodeTask			m_videoTask;
		FLVVAudioDecodeTask			m_audioTask;
	};
}


