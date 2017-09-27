#pragma once
#include "MShowCommon.h"
#include "MClientWindow.h"
#include "MPreviewView.h"
#include "MPreviewController.h"
#include "MAudioDSP.h"
#include "AudioAnalyser.h"
#include "audiosdk.h"
#include "MAudioQueue.h"
#include "MAppConfig.h"
#include "MAudioTest.h"
using namespace TinyUI;

using namespace pps::audiosdk;
#pragma comment(lib,"audiosdk.lib")
#pragma comment(lib,"fdkaac_dec.lib")
#pragma comment(lib,"fdkaac_enc.lib")

namespace MShow
{
	class MClientWindow;

	class MClientController
	{
	public:
		DISALLOW_COPY_AND_ASSIGN(MClientController)
	public:
		MClientController(MClientWindow& view);
		virtual ~MClientController();
		BOOL	Initialize();
		BOOL	Uninitialize();
		void	SetProgram(const string& szProgramName, const string& szProgramID);
		void	SetTimes(const string& szTime1, const string& szTime2);
		BOOL	SetPreview(const string& szPreviewURL);
	private:
		void InitializeUI();
		void OnMinimumClick(TinyVisual*, EventArgs& args);
		void OnCloseClick(TinyVisual*, EventArgs& args);
		void OnEditClick(TinyVisual*, EventArgs& args);
		void OnSaveClick(TinyVisual*, EventArgs& args);
		void OnCancelClick(TinyVisual*, EventArgs& args);
		void OnMicrophoneTestClick(TinyVisual*, EventArgs& args);
		void OnSpeakerTestClick(TinyVisual*, EventArgs& args);
		void OnMicrophoneFocus(TinyVisual*, FocusEventArgs& args);
		void OnSpeakerFocus(TinyVisual*, FocusEventArgs& args);
		void OnAudioDSP(BYTE* bits, LONG size);
		void OnMessagePump();
		static VOID CALLBACK OnTimer(PVOID lpParam, BOOLEAN TimerOrWaitFired);
	private:
		BOOL								m_bBreak;
		HANDLE								m_hTimer;
		LONGLONG							m_previousPTS;
		string								m_szProgramName;
		string								m_szProgramID;
		string								m_szPreviewURL;
		string								m_szName;//Ô´Ãû³Æ
		MClientWindow&						m_view;
		MAudioDSP							m_audioDSP;
		TinyScopedPtr<AudioSdk>				m_audioSDK;
		MAudioQueue							m_audioQueue;
		SpeakTest							m_speakTest;
		MicrophoneTest						m_microphoneTest;
		TinyScopedPtr<MPreviewController>	m_preview;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>		m_onMinimumClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>		m_onCloseClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>		m_onEditClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>		m_onSaveClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>		m_onCancelClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>		m_onMicrophoneTestClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>		m_onSpeakerTestClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, FocusEventArgs&)>> m_onSpeakerFocus;
		TinyScopedPtr<Delegate<void(TinyVisual*, FocusEventArgs&)>> m_onMicrophoneFocus;
	};

}

