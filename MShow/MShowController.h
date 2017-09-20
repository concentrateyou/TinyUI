#pragma once
#include "MShowWindow.h"
#include "MAudioDSP.h"
#include "AudioAnalyser.h"
#include "audiosdk.h"
#include "MAudioQueue.h"
#include "MAppConfig.h"
#include "MAudioTest.h"
#include <fstream> 
using namespace std;
using namespace Decode;

using namespace pps::audiosdk;
#pragma comment(lib,"audiosdk.lib")
#pragma comment(lib,"fdkaac_dec.lib")
#pragma comment(lib,"fdkaac_enc.lib")

namespace MShow
{
	/// <summary>
	/// Ö÷¿ØÖÆÆ÷
	/// </summary>
	class MShowController
	{
		DISALLOW_COPY_AND_ASSIGN(MShowController)
	public:
		MShowController(MShowWindow& window);
		~MShowController();
		BOOL	Initialize();
		void	Uninitialize();
		MAppConfig& AppConfig();
		MPreviewController* GetPreviewController();
	private:
		void InitializeUI();
		void OnAudio(BYTE* bits, LONG size, FLOAT ts, LPVOID);
		void OnAudioDSP(BYTE* bits, LONG size);
		void OnMessagePump();
	private:
		void OnPreviewClick(EventArgs&);
		void OnRecordClick(EventArgs&);
		void OnMinimumClick(EventArgs& args);
		void OnCloseClick(EventArgs& args);
		void OnMicrophoneTestClick(EventArgs& args);
		void OnSpeakerTestClick(EventArgs& args);
		void OnMicrophoneFocus(FocusEventArgs& args);
		void OnSpeakerFocus(FocusEventArgs& args);
	private:
		CLSID GetSpeakCLSID();
		CLSID GetMicrophoneCLSID();
	private:
		BOOL					m_bError;
		BOOL					m_bBreak;
		LONGLONG				m_previousPTS;
		TinyLock				m_lock;
		MShowWindow&			m_window;
		MAudioDSP				m_audioDSP;
		AudioAnalyser			m_audioAnalyser;
		TinyPerformanceTimer	m_timeQPC;
		TinyScopedPtr<AudioSdk>	m_audioSDK;
		MAudioQueue				m_audioQueue;
		MAppConfig				m_appConfig;
		TinyTaskBase			m_task;
		TinyEvent				m_event;
		SpeakTest				m_speakTest;
		MicrophoneTest			m_microphoneTest;
		TinyScopedPtr<MPreviewController>			m_preview;
		TinyScopedPtr<Delegate<void(EventArgs&)>>   m_onMinimumClick;
		TinyScopedPtr<Delegate<void(EventArgs&)>>   m_onCloseClick;
		TinyScopedPtr<Delegate<void(EventArgs&)>>   m_onPreviewClick;
		TinyScopedPtr<Delegate<void(EventArgs&)>>   m_onRecordClick;
		TinyScopedPtr<Delegate<void(EventArgs&)>>   m_onMicrophoneTestClick;
		TinyScopedPtr<Delegate<void(EventArgs&)>>   m_onSpeakerTestClick;
		TinyScopedPtr<Delegate<void(FocusEventArgs&)>>   m_onSpeakerFocus;
		TinyScopedPtr<Delegate<void(FocusEventArgs&)>>   m_onMicrophoneFocus;
	};
}


