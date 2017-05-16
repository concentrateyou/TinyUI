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

	class FLVDecode;
	class FLVAudioRender : public TinyTaskBase
	{
	public:
		FLVAudioRender(FLVDecode& decode);
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
		FLVDecode&		m_decode;
		TinySoundPlayer	m_player;
	};

	class FLVDecodeTask : public TinyTaskBase
	{
	public:
		FLVDecodeTask(FLVDecode& decode);
		~FLVDecodeTask();
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
		FLVVideoRender(FLVDecodeTask& decode);
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
		FLVDecodeTask&			m_decode;
		TinyPerformanceTimer	m_timer;
	};

	class FLVDecode : public TinyTaskBase
	{
		friend class FLVAudioRender;
		friend class FLVVideoRender;
		friend class FLVDecodeTask;
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
		FLVDecodeTask				m_decodeTask;
	};
}


