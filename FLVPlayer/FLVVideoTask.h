#pragma once
#include "FLVParse.h"
#include "FLVPlayer.h"
#include "IO/TinyTaskBase.h"
#include <concurrent_queue.h>
using namespace Decode;
using namespace TinyUI;
using namespace TinyUI::IO;

namespace FLVPlayer
{
	class FLVVideoTask : public TinyTaskBase
	{
	public:
		FLVVideoTask();
		~FLVVideoTask();
		BOOL	Submit(const TinySize& src, const TinySize& dst, BYTE* bits, LONG size);
		BOOL	Close(DWORD dwMs) OVERRIDE;
	private:
		void	OnMessagePump();
		void	OnH264(BYTE* bits, LONG size, LPVOID ps);
	private:
		TinyEvent	m_close;
		TinyScopedPtr<H264Decode>	m_h264;
	};
}

