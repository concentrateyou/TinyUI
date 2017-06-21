#pragma once
#include "MShowCommon.h"

namespace MShow
{
	class MRTMPPusher : public TinyTaskBase
	{
	public:
		MRTMPPusher();
		virtual ~MRTMPPusher();
		BOOL Submit();
		BOOL Close(DWORD dwMS) OVERRIDE;
	private:
		void OnMessagePump();
	private:
		BOOL	m_bClose;
	};

}

