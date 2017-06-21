#include "stdafx.h"
#include "MRTMPPusher.h"

namespace MShow
{
	MRTMPPusher::MRTMPPusher()
		:m_bClose(NULL)
	{
	}

	MRTMPPusher::~MRTMPPusher()
	{
	}

	BOOL MRTMPPusher::Submit()
	{
		m_bClose = FALSE;
		return TinyTaskBase::Submit(BindCallback(&MAudioRenderTask::OnMessagePump, this));
	}

	BOOL MRTMPPusher::Close(DWORD dwMS)
	{
		m_bClose = TRUE;
		if (TinyTaskBase::Close(dwMS))
		{
			return TRUE;
		}
		return FALSE;
	}

	void MRTMPPusher::OnMessagePump()
	{

	}
}
