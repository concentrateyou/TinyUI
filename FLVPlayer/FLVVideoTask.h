#pragma once
#include "FLVParse.h"
#include "FLVPlayer.h"
#include "Common/TinyTime.h"
#include "IO/TinyTaskBase.h"
#include <queue>
using namespace Decode;
using namespace TinyUI;
using namespace TinyUI::IO;

namespace FLVPlayer
{
	class FLVDecodeTask;

	class FLVVideoTask : public TinyTaskBase
	{
	public:
		FLVVideoTask(FLVDecodeTask* pTask, TinyLock& lock, HWND hWND);
		~FLVVideoTask();
		BOOL	Submit(const TinySize& srcsize, const TinySize& dstsize, BYTE* bits, LONG size);
		BOOL	Close(DWORD dwMs) OVERRIDE;
		void	Push(const SampleTag& packet);
	private:
		void	OnMessagePump();
		BOOL	OnProcessTag(LONG& pts);
		void	OnRender(BYTE* bits, LONG size);
	private:
		LONG						m_currentPTS;
		TinyLock&					m_lock;
		TinyEvent					m_wait;
		TinySize					m_dstsize;
		HWND						m_hWND;
		std::queue<SampleTag>		m_queue;
		TinyEvent					m_close;
		FLVDecodeTask*				m_pTask;
		TinyPerformanceTimer		m_timer;
		TinyScopedPtr<H264Decode>	m_h264;
	};
}

