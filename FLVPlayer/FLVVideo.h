#pragma once
#include "FLVParser.h"
#include "FLVPlayer.h"
#include "Common/TinyTime.h"
#include "IO/TinyTaskBase.h"
#include <queue>
using namespace Decode;
using namespace TinyUI;
using namespace TinyUI::IO;

namespace FLVPlayer
{
	class FLVRender;
	class FLVVideo : public TinyTaskBase
	{
		friend class FLVRender;
	public:
		FLVVideo(HWND hWND);
		~FLVVideo();
		BOOL	Submit();
		BOOL	Close(DWORD dwMs) OVERRIDE;
	private:
		void	OnMessagePump();
	private:
		TinyScopedPtr<FLVRender>	m_render;
		TinySize					m_size;
		DWORD						m_rate;
		HWND						m_hWND;
		TinyEvent					m_close;
		TinyScopedPtr<H264Decode>	m_h264;
		FLVReader					m_reader;
		std::queue<SampleTag>		m_queue;
		DWORD						m_queueSize;
		TinyLock					m_lock;
	};

	class FLVRender : public TinyTaskBase
	{
	public:
		FLVRender(FLVVideo& video);
	public:
		BOOL	Submit();
		BOOL	Close(DWORD dwMs) OVERRIDE;
	private:
		void	OnMessagePump();
		void	OnRender(BYTE* bits, LONG size);
	private:
		FLVVideo&	m_video;
		LONG		m_currentPTS;
		TinyPerformanceTimer		m_timer;
	};
}

