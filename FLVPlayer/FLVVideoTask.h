#pragma once
#include "FLVParse.h"
#include "FLVPlayer.h"
#include "Common/TinyTime.h"
#include "IO/TinyTaskBase.h"
using namespace Decode;
using namespace TinyUI;
using namespace TinyUI::IO;

namespace FLVPlayer
{
	class FLVDecodeTask;

	class FLVVideoTask : public TinyTaskBase
	{
	public:
		FLVVideoTask(FLVDecodeTask* pTask, HWND hWND);
		~FLVVideoTask();
		BOOL	Submit(const TinySize& src, const TinySize& dst, BYTE* bits, LONG size);
		BOOL	Close(DWORD dwMs) OVERRIDE;
	private:
		void	OnMessagePump();
		void	OnH264(BYTE* bits, LONG size, LPVOID ps);
	private:
		INT							m_currentPTS;
		INT							m_framePTS;
		INT							m_frameDTS;
		TinySize					m_dst;
		HWND						m_hWND;
		HBITMAP						m_hBitmap;
		TinyEvent					m_wait;
		TinyEvent					m_close;
		FLVDecodeTask*				m_pTask;
		TinyPerformanceTimer		m_timer;
		TinyScopedPtr<H264Decode>	m_h264;
	};
}

