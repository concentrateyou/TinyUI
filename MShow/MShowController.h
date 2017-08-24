#pragma once
#include "MShowWindow.h"
#include "MAudioCapture.h"
#include "MAudioDSP.h"
#include "AudioAnalyser.h"
#include "audiosdk.h"
#include "MAudioQueue.h"
#include "MAppConfig.h"
#include <fstream> 
using namespace std;
using namespace DShow;
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
		void OnPreview(void*, INT);
		void OnRecord(void*, INT);
		void OnAudio(BYTE* bits, LONG size, FLOAT ts, LPVOID);
		void OnAudioDSP(BYTE* bits, LONG size);
		void OnMessagePump();
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
		TinyScopedPtr<MPreviewController>			m_preview;
		TinyScopedPtr<Delegate<void(void*, INT)>>	m_onPreviewClick;
		TinyScopedPtr<Delegate<void(void*, INT)>>	m_onRecordClick;
	};
}


