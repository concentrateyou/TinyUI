#include "stdafx.h"
#include "MRTMPPusher.h"

namespace MShow
{
#define MAX_PUSH_QUEUE_SIZE (1024 * 1024)

	MRTMPPusher::MRTMPPusher()
		:m_bClose(FALSE),
		m_dwReconnect(0)
	{
	}

	MRTMPPusher::~MRTMPPusher()
	{
	}

	BOOL MRTMPPusher::Connect(const TinyString& szURL, DWORD dwRate)
	{
		m_dwRate = dwRate;
		if (m_client.Connect(szURL))
		{
			return TRUE;
		}
		return FALSE;
	}

	BOOL MRTMPPusher::Submit()
	{
		m_bClose = FALSE;
		return TinyTaskBase::Submit(BindCallback(&MRTMPPusher::OnMessagePump, this));
	}

	BOOL MRTMPPusher::Close(DWORD dwMS)
	{
		m_bClose = TRUE;
		m_dwReconnect = 0;
		if (TinyTaskBase::Close(dwMS))
		{
			m_client.Disconnect();
			return TRUE;
		}
		return FALSE;
	}

	void MRTMPPusher::OnMessagePump()
	{
		DWORD dwMS = 1000 / m_dwRate;
		SampleTag sampleTag = { 0 };
		for (;;)
		{
			if (m_bClose)
				break;
			/*INT size = m_queue.GetSize();
			if (size > MAX_PUSH_QUEUE_SIZE)
			{
				Sleep(3);
				continue;
			}
			ZeroMemory(&sampleTag, sizeof(sampleTag));*/

		}
	}
}
