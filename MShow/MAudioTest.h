#pragma once
#include "MAudioDSP.h"
#include "MAudioQueue.h"
#include "MAppConfig.h"
#include "MPacketQueue.h"
#include "MClientWindow.h"

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
		void Shutdown();
	private:
		void OnMessagePump();
	private:
		BOOL				m_bPlaying;
		TinyEvent			m_events[1];
		TinySoundPlayer		m_player;
		TinyWaveFile		m_waveFile;
		TinyTask			m_task;
	};
	/// <summary>
	/// Âó¿Ë·ç²âÊÔ
	/// </summary>
	class MicrophoneTest
	{
		DISALLOW_COPY_AND_ASSIGN(MicrophoneTest)
	public:
		MicrophoneTest(MClientWindow& view);
		~MicrophoneTest();
		BOOL Invoke(const GUID& guid, HWND hWND);
		void Shutdown();
	private:
		BOOL InvokeInternal(const GUID& guid, HWND hWND);
	private:
		void OnMessagePump();
		void OnAudioCapture(BYTE*, LONG, INT);
	private:
		BOOL					m_bBreak;
		BOOL					m_bCapturing;
		MClientWindow&			m_view;
		TinyXAudio				m_audio;
		MAudioDSP				m_audioDSP;
		TinyTask				m_task;
		MPacketAllocQueue		m_queue;
	};
}

