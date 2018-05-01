#include "stdafx.h"
#include "WavePlayer.h"


WavePlayer::WavePlayer()
{
}


WavePlayer::~WavePlayer()
{
}

BOOL WavePlayer::Submit(HWND hWND)
{
	m_hWND = hWND;
	m_player.Close();
	if (!m_player.Open(hWND))
		return FALSE;
	if (!m_waveFile.Open("D:\\Íõ·Æ - ´Ò´ÒÄÇÄê.wav"))
		return FALSE;
	if (!m_player.SetFormat(m_waveFile.GetFormat(), 4096 * 3))
		return FALSE;
	m_events[0].CreateEvent(TRUE, FALSE);
	m_events[1].CreateEvent(TRUE, FALSE);
	m_events[2].CreateEvent(TRUE, FALSE);
	DSBPOSITIONNOTIFY vals[3];
	vals[0].dwOffset = 4096 - 1;
	vals[0].hEventNotify = m_events[0];
	vals[1].dwOffset = 4096 * 2 - 1;
	vals[1].hEventNotify = m_events[1];
	vals[2].dwOffset = 4096 * 3 - 1;
	vals[2].hEventNotify = m_events[2];
	m_player.SetNotifys(3, vals);
	m_analyser.Initialize();
	return TinyThread::Submit(BindCallback(&WavePlayer::OnMessagePump, this));
}

void WavePlayer::OnMessagePump()
{
	DWORD	dwOffset = 0;
	BYTE data[4096];
	m_player.Play();
	for (;;)
	{
		HANDLE handles[3] = { m_events[0],m_events[1],m_events[2] };
		HRESULT hRes = WaitForMultipleObjects(3, handles, FALSE, INFINITE);
		switch (hRes)
		{
		case WAIT_OBJECT_0:
			dwOffset = 0;
			break;
		case WAIT_OBJECT_0 + 1:
			dwOffset = 4096;
			break;
		case WAIT_OBJECT_0 + 2:
			dwOffset = 4096 * 2;
			break;
		}
		LONG numberOfBytesRead = 0;
		if (m_waveFile.Read(data, 4096, &numberOfBytesRead))
		{
			m_player.Fill(data, numberOfBytesRead, dwOffset);
			m_analyser.Process(m_hWND, data, numberOfBytesRead);
		}
	}
}


BOOL WavePlayer::Close(DWORD dwMS)
{
	m_player.Close();
	m_waveFile.Close();
	return TinyThread::Close(dwMS);
}