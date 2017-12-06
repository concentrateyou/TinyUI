#pragma once
#include "MAudioDSP.h"
#include "MAudioQueue.h"
#include "MAppConfig.h"
#include "MPacketQueue.h"

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
		void OnAudioCapture(BYTE*, LONG, INT);
	private:
		BOOL					m_bBreak;
		BOOL					m_bCapturing;
		TinyXAudio				m_audio;
		MAudioDSP				m_audioDSP;
		TinyTaskBase			m_task;
		MPacketAllocQueue		m_queue;
	};
}

