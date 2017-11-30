#pragma once
#include "MAudioDSP.h"
#include "AudioAnalyser.h"
#include "audiosdk.h"
#include "MAudioQueue.h"
#include "MAppConfig.h"

namespace MShow
{
	/// <summary>
	/// ÑïÉùÆ÷²âÊÔ
	/// </summary>
	class SpeakTest
	{
		DISALLOW_COPY_AND_ASSIGN(SpeakTest)
	public:
		SpeakTest();
		~SpeakTest();
		BOOL Invoke(const TinyString& szFile, const GUID& guid, HWND hWND);
		BOOL Shutdown();
	private:
		void OnMessagePump();
	private:
		BOOL				m_playing;
		TinyEvent			m_events[1];
		TinySoundPlayer		m_player;
		TinyWaveFile		m_waveFile;
		TinyTaskBase		m_task;
	};
	/// <summary>
	/// Âó¿Ë·ç²âÊÔ
	/// </summary>
	class MicrophoneTest
	{
		DISALLOW_COPY_AND_ASSIGN(MicrophoneTest)
	public:
		MicrophoneTest();
		~MicrophoneTest();
		BOOL Invoke(const GUID& guid, HWND hWND);
		BOOL Shutdown();
	private:
		void OnMessagePump();
		void OnAudioCapture();
		void OnAudioPlay();
	private:
		BOOL					m_bFirst;
		BOOL					m_capturing;
		DWORD					m_dwPlayOffset;
		TinyEvent				m_events[4];
		TinySoundCapture		m_capture;
		TinySoundPlayer			m_player;
		TinyTaskBase			m_task;
		TinyBufferArray<BYTE>	m_buffer;
	};
}

