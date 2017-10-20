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
		MPreviewController* GetPreviewController();
	private:
		void InitializeUI();
		void OnAudio(BYTE* bits, LONG size, FLOAT ts, LPVOID);
		void OnAudioDSP(BYTE* bits, LONG size);
		void OnMessagePump();
	private:
		void OnPreviewClick(TinyVisual*, EventArgs&);
		void OnRecordClick(TinyVisual*, EventArgs&);
		void OnMinimumClick(TinyVisual*, EventArgs& args);
		void OnCloseClick(TinyVisual*, EventArgs& args);
		void OnMicrophoneTestClick(TinyVisual*, EventArgs& args);
		void OnSpeakerTestClick(TinyVisual*, EventArgs& args);
		void OnMicrophoneFocus(TinyVisual*, FocusEventArgs& args);
		void OnSpeakerFocus(TinyVisual*, FocusEventArgs& args);
	private:
		CLSID GetSpeakCLSID();
		CLSID GetMicrophoneCLSID();
	private:
		BOOL					m_bError;
		BOOL					m_bBreak;
		INT						m_sourceID;
		LONGLONG				m_previousPTS;
		TinyLock				m_lock;
		MShowWindow&			m_window;
		MAudioDSP				m_audioDSP;
		AudioAnalyser			m_audioAnalyser;
		TinyPerformanceTimer	m_timeQPC;
		TinyScopedPtr<AudioSdk>	m_audioSDK;
		MAudioQueue				m_audioQueue;
		TinyTaskBase			m_task;
		TinyEvent				m_event;
		SpeakTest				m_speakTest;
		MicrophoneTest			m_microphoneTest;
		TinyScopedPtr<MPreviewController>			m_preview;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>   m_onMinimumClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>   m_onCloseClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>   m_onPreviewClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>   m_onRecordClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>   m_onMicrophoneTestClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>   m_onSpeakerTestClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, FocusEventArgs&)>>   m_onSpeakerFocus;
		TinyScopedPtr<Delegate<void(TinyVisual*, FocusEventArgs&)>>   m_onMicrophoneFocus;
	};
}


