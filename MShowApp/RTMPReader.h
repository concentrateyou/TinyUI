#pragma once
#include "MShowCommon.h"

namespace MShow
{
	/// <summary>
	/// RTMPÍøÂçÁ÷²¥·ÅÆ÷
	/// </summary>
	class RTMPReader : public TinyTaskBase
	{
		DISALLOW_COPY_AND_ASSIGN(RTMPReader)
	public:
		RTMPReader();
		~RTMPReader();
		BOOL Open(LPCSTR pzURL);
		BOOL Close(DWORD dwMs) OVERRIDE;
	private:
		void OnMessagePump();
	private:
		RTMP		m_sRTMP;
		TinyEvent	m_close;
	};
}


