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
		:m_bCapturing(FALSE),
		m_bBreak(FALSE)
	{

	}
	MicrophoneTest::~MicrophoneTest()
	{

	}
	BOOL MicrophoneTest::Invoke(const GUID& clsid, HWND hWND)
	{
		if (m_bCapturing)
			return TRUE;
		WAVEFORMATEX waveFMT = { 0 };
		waveFMT.cbSize = 0;
		waveFMT.nChannels = 2;
		waveFMT.wBitsPerSample = 16;
		waveFMT.nSamplesPerSec = 44100;
		waveFMT.wFormatTag = WAVE_FORMAT_PCM;
		waveFMT.nBlockAlign = 4;
		waveFMT.nAvgBytesPerSec = 176400;
		m_audio.Close();
		if (!m_audio.Open(&waveFMT))
		{
			LOG(ERROR) << "[MicrophoneTest] Invoke Audio Open FAIL\n";
			return FALSE;
		}
		vector<PLAYDEVICE> devices;
		TinySoundPlayer::Enumerate(devices);
		if (devices.size() <= 0)
		{
			LOG(ERROR) << "[MicrophoneTest] Invoke Enumerate Audio FAIL\n";
			return FALSE;
		}
		m_audioDSP.AllowDB(FALSE);
		if (!m_audioDSP.Initialize(BindCallback(&MicrophoneTest::OnAudioCapture, this)))
		{
			LOG(ERROR) << "[MicrophoneTest] Invoke AudioDSP Initialize FAIL\n";
			return FALSE;
		}
		if (!m_audioDSP.Open(clsid, devices[0].Guid))
		{
			LOG(ERROR) << "[MicrophoneTest] Invoke AudioDSP Open FAIL\n";
			return FALSE;
		}
		m_bBreak = FALSE;
		if (!m_task.Submit(BindCallback(&MicrophoneTest::OnMessagePump, this)))
		{
			LOG(ERROR) << "[MicrophoneTest] Invoke Submit FAIL\n";
			return FALSE;
		}
		m_audio.Start();
		m_audioDSP.Stop();
		m_audioDSP.Start();
		return TRUE;
	}
	BOOL MicrophoneTest::Shutdown()
	{
		m_audio.Close();
		m_audioDSP.Stop();
		m_audioDSP.Close();
		m_bBreak = TRUE;
		m_task.Close(INFINITE);
		m_queue.RemoveAll();
		m_bCapturing = FALSE;
		return TRUE;
	}
	void MicrophoneTest::OnMessagePump()
	{
		SampleTag sampleTag;
		for (;;)
		{
			if (m_bBreak)
				break;
			ZeroMemory(&sampleTag, sizeof(sampleTag));
			if (!m_queue.Pop(sampleTag))
			{
				Sleep(5);
				continue;
			}
			m_audio.Play(sampleTag.bits + 4, sampleTag.size, 5000);
			m_queue.Free(sampleTag.bits);
		}
	}
	void MicrophoneTest::OnAudioCapture(BYTE* bits, LONG size, INT db)
	{
		SampleTag sampleTag;
		ZeroMemory(&sampleTag, sizeof(sampleTag));
		if (size == 4096)
		{

			if (m_queue.GetAllocSize() == 0)
			{
				INT count = MAX_AUDIO_QUEUE_SIZE / size + 1;
				m_queue.Initialize(count, size + 4);
			}
			sampleTag.size = size;
			sampleTag.bits = static_cast<BYTE*>(m_queue.Alloc());
			memcpy(sampleTag.bits + 4, bits, size);
			m_queue.Push(sampleTag);
		}
	}
}
