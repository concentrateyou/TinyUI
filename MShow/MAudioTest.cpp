#include "stdafx.h"
#include "MAudioTest.h"

namespace MShow
{
	SpeakTest::SpeakTest()
		:m_playing(FALSE)
	{
		m_events[0].CreateEvent();
	}
	SpeakTest::~SpeakTest()
	{

	}
	BOOL SpeakTest::Invoke(const TinyString& szFile, const GUID& clsid, HWND hWND)
	{
		if (m_playing)
			return TRUE;
		m_waveFile.Close();
		m_player.Close();
		if (!m_waveFile.Open(szFile.STR()))
		{
			LOG(ERROR) << "[SpeakTest] Wave Open File " << szFile.STR() << " Fail";
			return FALSE;
		}
		if (!m_player.Open(clsid, hWND))
		{
			LOG(ERROR) << "[SpeakTest] Play Open Fail";
			return FALSE;
		}
		LONG dataSize = m_waveFile.GetSize();
		if (!m_player.SetFormat(m_waveFile.GetFormat(), dataSize))
		{
			LOG(ERROR) << "[SpeakTest] Play SetFormat Fail";
			return FALSE;
		}
		DSBPOSITIONNOTIFY vals[1];
		vals[0].dwOffset = DSBPN_OFFSETSTOP;
		vals[0].hEventNotify = m_events[0];
		m_player.SetNotifys(1, vals);
		TinyScopedArray<BYTE> bits(new BYTE[dataSize]);
		LONG size = 0;
		m_waveFile.Read(bits, m_waveFile.GetSize(), &size);
		m_player.Fill(bits, size, 0);
		m_player.Play(0);
		m_playing = m_task.Submit(BindCallback(&SpeakTest::OnMessagePump, this));
		LOG(INFO) << "[SpeakTest] Invoke OK";
		return TRUE;
	}
	BOOL SpeakTest::Shutdown()
	{
		m_player.Stop();
		m_player.Close();
		m_waveFile.Close();
		m_task.Close(INFINITE);
		m_playing = FALSE;
		return TRUE;
	}
	void SpeakTest::OnMessagePump()
	{
		for (;;)
		{
			HANDLE handles[1] = { m_events[0] };
			HRESULT hRes = WaitForMultipleObjects(1, handles, FALSE, INFINITE);
			if (hRes == WAIT_OBJECT_0)
			{
				LOG(INFO) << "[SpeakTest] Play Finish";
				m_playing = FALSE;
				break;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	MicrophoneTest::MicrophoneTest()
		:m_capturing(FALSE),
		m_bFirst(FALSE),
		m_dwPlayOffset(0)
	{
		m_events[0].CreateEvent();
		m_events[1].CreateEvent();
		m_events[2].CreateEvent();
		m_events[3].CreateEvent();
	}
	MicrophoneTest::~MicrophoneTest()
	{

	}
	BOOL MicrophoneTest::Invoke(const GUID& clsid, HWND hWND)
	{
		if (m_capturing)
			return TRUE;
		m_capture.Close();
		m_player.Close();
		WAVEFORMATEX waveFMT = { 0 };
		waveFMT.cbSize = 0;
		waveFMT.nChannels = 2;
		waveFMT.wBitsPerSample = 16;
		waveFMT.nSamplesPerSec = 44100;
		waveFMT.wFormatTag = WAVE_FORMAT_PCM;
		waveFMT.nBlockAlign = 4;
		waveFMT.nAvgBytesPerSec = 176400;
		if (!m_capture.Open(clsid))
		{
			LOG(ERROR) << "[MicrophoneTest] Capture Open Fail";
			goto _ERROR;
		}
		if (!m_capture.SetFormat(&waveFMT, 4096 * 2))
		{
			LOG(ERROR) << "[MicrophoneTest] Capture SetFormat Fail";
			goto _ERROR;
		}
		DSBPOSITIONNOTIFY vals[2];
		vals[0].dwOffset = 4096 * 1 - 1;
		vals[0].hEventNotify = m_events[0];
		vals[1].dwOffset = 4096 * 2 - 1;
		vals[1].hEventNotify = m_events[1];
		if (!m_capture.SetNotifys(2, vals))
		{
			LOG(ERROR) << "[MicrophoneTest] Capture SetNotifys Fail";
			goto _ERROR;
		}

		if (!m_player.Open(hWND))
		{
			LOG(ERROR) << "[MicrophoneTest] Player Open Fail";
			goto _ERROR;
		}
		if (!m_player.SetFormat(&waveFMT, 4096 * 2))
		{
			LOG(ERROR) << "[MicrophoneTest] Player Open Fail";
			goto _ERROR;
		}
		DSBPOSITIONNOTIFY vals1[1];
		vals1[0].dwOffset = 4096 * 2 - 1;
		vals1[0].hEventNotify = m_events[2];
		if (!m_player.SetNotifys(1, vals1))
		{
			LOG(ERROR) << "[MicrophoneTest] Player SetNotifys Fail";
			goto _ERROR;
		}
		if (!m_player.Play())
		{
			LOG(ERROR) << "[MicrophoneTest] Player Play Fail";
			goto _ERROR;
		}
		if (!m_capture.Start())
		{
			LOG(ERROR) << "[MicrophoneTest] Capture Start Fail";
			goto _ERROR;
		}
		m_capturing = m_task.Submit(BindCallback(&MicrophoneTest::OnMessagePump, this));
		LOG(INFO) << "[MicrophoneTest] Invoke OK";
		return TRUE;
	_ERROR:
		m_player.Close();
		m_capture.Close();
		return FALSE;
	}
	BOOL MicrophoneTest::Shutdown()
	{
		m_capturing = FALSE;
		m_events[3].SetEvent();
		if (m_task.Close(INFINITE))
		{
			m_capture.Stop();
			m_capture.Close();
			m_player.Stop();
			m_player.Close();
			return TRUE;
		}
		return FALSE;
	}
	void MicrophoneTest::OnMessagePump()
	{
		for (;;)
		{
			HANDLE handles[4] = { m_events[0],m_events[1],m_events[2],m_events[3] };
			HRESULT hRes = WaitForMultipleObjects(4, handles, FALSE, INFINITE);
			if (hRes == (WAIT_OBJECT_0 + 3))
				break;
			switch (hRes)
			{
			case WAIT_OBJECT_0:
			case WAIT_OBJECT_0 + 1:
			{
				OnAudioCapture();
				if (!m_bFirst)
				{
					m_bFirst = TRUE;
					OnAudioPlay();
				}
			}
			break;
			case WAIT_OBJECT_0 + 2:
			{
				OnAudioPlay();
			}
			break;
			}
		}
	}
	void MicrophoneTest::OnAudioCapture()
	{
		BYTE* ps = NULL;
		LONG size = 0;
		if (m_capture.GetBuffer(ps, size, 0))
		{
			m_buffer.Add(ps, size);
		}
	}
	void MicrophoneTest::OnAudioPlay()
	{
		if (m_buffer.GetSize() >= 8192)
		{
			m_player.Fill(m_buffer.GetPointer(), 8192, 0);
			m_buffer.Remove(0, 8192);
		}
	}
}
