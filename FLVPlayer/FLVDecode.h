#pragma once
#include "FLVParser.h"
#include "FLVPlayer.h"
#include "IO/TinyTaskBase.h"
#include "FLVAudioTask.h"
#include "FLVVideoTask.h"
#include <queue>
using namespace Decode;
using namespace TinyUI;
using namespace TinyUI::IO;

namespace FLVPlayer
{
	class FLVDecode : public TinyTaskBase
	{
	public:
		FLVDecode(HWND hWND);
		~FLVDecode();
		BOOL	Submit();
		BOOL	Close(DWORD dwMs) OVERRIDE;
	private:
		void	OnMessagePump();
	private:
		HWND						m_hWND;
	};
}


