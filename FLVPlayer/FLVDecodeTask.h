#pragma once
#include "FLVParse.h"
#include "IO/TinyTaskBase.h"
using namespace Decode;

namespace FLVPlayer
{
	class FLVDecodeTask : public TinyTaskBase
	{
	public:
		FLVDecodeTask();
		~FLVDecodeTask();
		BOOL	Submit();
		BOOL	Close(DWORD dwMs) OVERRIDE;
	private:
		void	OnMessagePump();
	};
}


