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
		void	SetProgram(const string& szProgramName, const string& szProgramID, const string& szLogID);
		void	SetTimes(const string& szTime1, const string& szTime2);
		BOOL	SetPreview(const string& szPreviewURL);
		void	UpdateMicrophones();
		void	UpdateSpeakers();
	private:
		void	InitializeUI();
		void	OnSettingClick(TinyVisual*, EventArgs& args);
		void	OnMinimumClick(TinyVisual*, EventArgs& args);
		void	OnCloseClick(TinyVisual*, EventArgs& args);
		void	OnEditClick(TinyVisual*, EventArgs& args);
		void	OnSaveClick(TinyVisual*, EventArgs& args);
		void	OnCancelClick(TinyVisual*, EventArgs& args);
		void	OnStartCommentaryClick(TinyVisual*, EventArgs& args);
		void	OnPauseCommentaryClick(TinyVisual*, EventArgs& args);
		void	OnStopCommentaryClick(TinyVisual*, EventArgs& args);
		void	OnMicrophoneTestClick(TinyVisual*, EventArgs& args);
		void	OnSpeakerTestClick(TinyVisual*, EventArgs& args);
		void	OnMicrophoneFocus(TinyVisual*, FocusEventArgs& args);
		void	OnSpeakerFocus(TinyVisual*, FocusEventArgs& args);
		void	OnMessagePump();
		BOOL	Query(const string& sourceID, INT& count);
	private:
		void	OnAudio(BYTE* bits, LONG size);
		void	OnTimerStatus();
		void	OnTry();
	private:
		BOOL		Connect();//添加源
		BOOL		Disconnect(const string& sourceID, BOOL del = FALSE);
		BOOL		UpdatePreviewURL(const string& sourceID, const string& strURL);
		BOOL		UpdateName(const string& sourceID, const string& strName);
		BOOL		GetPreviewURL(string& szURL, INT& iAudio, string& szIP);
		void		Close();
		BOOL		StartCommentary();
		void		StopCommentary();
		CLSID		GetSpeakCLSID();
		CLSID		GetMicrophoneCLSID();
	private:
		BOOL								m_bPause;
		BOOL								m_bCommentarying;
		BOOL								m_bBreak;
		TinyTaskTimer						m_timerStatus;//检测解说状态
		LONGLONG							m_previousPTS;
		string								m_szURL;//预览流地址
		string								m_szSourceID;
		string								m_szLogID;
		string								m_szProgramName;
		string								m_szProgramID;
		string								m_szPreviewURL;
		string								m_szName;//源名称
		MClientWindow&						m_view;
		SpeakTest							m_speakTest;
		MicrophoneTest						m_microphoneTest;
		MAudioQueue							m_audioQueue;
		MAudioDSP							m_audioDSP;
		TinyScopedPtr<AudioSdk>				m_audioSDK;
		TinyTaskBase						m_task;
		TinyTaskTimer						m_taskTimer;
		TinyPerformanceTime					m_timeQPC;
		TinyScopedPtr<MPreviewController>	m_preview;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>		m_onSettingClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>		m_onMinimumClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>		m_onCloseClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>		m_onEditClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>		m_onSaveClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>		m_onCancelClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>		m_onStartCommentaryClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>		m_onStopCommentaryClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>		m_onPauseCommentaryClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>		m_onMicrophoneTestClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>		m_onSpeakerTestClick;
		TinyScopedPtr<Delegate<void(TinyVisual*, FocusEventArgs&)>> m_onSpeakerFocus;
		TinyScopedPtr<Delegate<void(TinyVisual*, FocusEventArgs&)>> m_onMicrophoneFocus;
	};
}

