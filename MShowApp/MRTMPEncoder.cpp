#include "stdafx.h"
#include "MRTMPEncoder.h"

namespace MShow
{
	MRTMPEncoder::MRTMPEncoder(MPreviewController& controller)
		:m_controller(controller)
	{

	}

	MRTMPEncoder::~MRTMPEncoder()
	{

	}

	BOOL MRTMPEncoder::Submit()
	{
		m_bClose = FALSE;
		return TinyTaskBase::Submit(BindCallback(&MRTMPEncoder::OnMessagePump, this));
	}

	BOOL MRTMPEncoder::Close(DWORD dwMS)
	{
		m_bClose = TRUE;
		if (TinyTaskBase::Close(dwMS))
		{

			return TRUE;
		}
		return FALSE;
	}

	void MRTMPEncoder::OnMessagePump()
	{
		//DWORD dwMS = 1000 / m_dwRate;
		SampleTag sampleTag = { 0 };
		for (;;)
		{
			if (m_bClose)
				break;
		}
	}
}
