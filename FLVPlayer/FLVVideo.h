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
	class FLVVideo : public TinyTaskBase
	{
	public:
		FLVVideo(HWND hWND);
		~FLVVideo();
		BOOL	Submit();
		BOOL	Close(DWORD dwMs) OVERRIDE;
	private:
		void	OnMessagePump();
		void	OnRender(BYTE* bits, LONG size);
	private:
		TinyPerformanceTimer		m_timer;
		TinySize					m_size;
		DWORD						m_rate;
		LONG						m_currentPTS;
		HWND						m_hWND;
		TinyEvent					m_close;
		TinyScopedPtr<H264Decode>	m_h264;
		FLVReader					m_reader;
	};
}

