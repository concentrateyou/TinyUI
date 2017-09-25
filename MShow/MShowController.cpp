#include "stdafx.h"
#include "MShowController.h"
#include "MPreviewController.h"
#include "MShow.h"

namespace MShow
{
	MShowController::MShowController(MShowWindow& window)
		:m_window(window),
		m_previousPTS(0),
		m_bBreak(FALSE),
		m_bError(FALSE)
	{
		m_event.CreateEvent();
	}

	MShowController::~MShowController()
	{

	}

	BOOL MShowController::Initialize()
	{
		m_preview.Reset(new MPreviewController(m_window.m_previewView));
		if (!m_preview)
			return FALSE;
		if (!m_preview->Initialize())
		{
			LOG(ERROR) << "Preview Initialize Fail";
			return FALSE;
		}
		if (!m_audioDSP.Initialize(BindCallback(&MShowController::OnAudioDSP, this)))
		{
			LOG(ERROR) << "AudioDSP Initialize Fail";
			return FALSE;
		}
		if (!m_audioAnalyser.Initialize())
		{
			LOG(ERROR) << "AudioAnalyser Initialize Fail";
			return FALSE;
		}
		InitializeUI();
		return TRUE;
	}
	void MShowController::InitializeUI()
	{
		TinyVisual* visual = m_window.GetDocument()->GetVisualByName("sysmin");
		if (visual != NULL)
		{
			m_onMinimumClick.Reset(new Delegate<void(TinyVisual*, EventArgs&)>(this, &MShowController::OnMinimumClick));
			visual->EVENT_CLICK += m_onMinimumClick;
		}
		visual = m_window.GetDocument()->GetVisualByName("sysclose");
		if (visual != NULL)
		{
			m_onCloseClick.Reset(new Delegate<void(TinyVisual*, EventArgs&)>(this, &MShowController::OnCloseClick));
			visual->EVENT_CLICK += m_onCloseClick;
		}
		visual = m_window.GetDocument()->GetVisualByName("previewWND");
		if (visual != NULL && visual->IsKindOf(RUNTIME_CLASS(TinyVisualNative)))
		{
			TinyVisualNative* val = static_cast<TinyVisualNative*>(visual);
			val->SetWindow(&m_window.m_previewView);
		}
		visual = m_window.GetDocument()->GetVisualByName("btnPreview");
		if (visual != NULL)
		{
			m_onPreviewClick.Reset(new Delegate<void(TinyVisual*, EventArgs&)>(this, &MShowController::OnPreviewClick));
			visual->EVENT_CLICK += m_onPreviewClick;
		}
		visual = m_window.GetDocument()->GetVisualByName("btnRecord");
		if (visual != NULL)
		{
			m_onRecordClick.Reset(new Delegate<void(TinyVisual*, EventArgs&)>(this, &MShowController::OnRecordClick));//开始录音
			visual->EVENT_CLICK += m_onRecordClick;
		}
		visual = m_window.GetDocument()->GetVisualByName("txtURL");
		if (visual != NULL && visual->IsKindOf(RUNTIME_CLASS(TinyVisualTextBox)))
		{
			TinyVisualTextBox* val = static_cast<TinyVisualTextBox*>(visual);
			val->SetText("http://10.110.48.109:4330/6706763023");
		}
		vector<CAPTUREDEVICE> captures;
		TinySoundCapture::Enumerate(captures);
		visual = m_window.GetDocument()->GetVisualByName("microphone");
		if (visual != NULL && visual->IsKindOf(RUNTIME_CLASS(TinyVisualComboBox)))
		{
			TinyVisualComboBox* val = static_cast<TinyVisualComboBox*>(visual);
			for (INT i = 0;i < captures.size();i++)
			{
				wstring szGUID;
				szGUID.resize(39);
				::StringFromGUID2(captures[i].Guid, &szGUID[0], 39);
				string sz = WStringToString(szGUID);
				val->AddOption(sz.c_str(), captures[i].Description.c_str());
			}
			val->SetSelected(0);
		}
		vector<PLAYDEVICE> renders;
		TinySoundPlayer::Enumerate(renders);
		visual = m_window.GetDocument()->GetVisualByName("speaker");
		if (visual != NULL && visual->IsKindOf(RUNTIME_CLASS(TinyVisualComboBox)))
		{
			TinyVisualComboBox* val = static_cast<TinyVisualComboBox*>(visual);
			for (INT i = 0;i < renders.size();i++)
			{
				wstring szGUID;
				szGUID.resize(39);
				::StringFromGUID2(renders[i].Guid, &szGUID[0], 39);
				string sz = WStringToString(szGUID);
				val->AddOption(sz.c_str(), renders[i].Description.c_str());
			}
			val->SetSelected(0);
		}
		visual = m_window.GetDocument()->GetVisualByName("btnMicrophoneTest");
		if (visual != NULL)
		{
			m_onMicrophoneTestClick.Reset(new Delegate<void(TinyVisual*, EventArgs&)>(this, &MShowController::OnMicrophoneTestClick));
			visual->EVENT_CLICK += m_onMicrophoneTestClick;
			m_onMicrophoneFocus.Reset(new Delegate<void(TinyVisual*, FocusEventArgs&)>(this, &MShowController::OnMicrophoneFocus));
			visual->EVENT_FOCUS += m_onMicrophoneFocus;
		}
		visual = m_window.GetDocument()->GetVisualByName("btnSpeakerTest");
		if (visual != NULL)
		{
			m_onSpeakerTestClick.Reset(new Delegate<void(TinyVisual*, EventArgs&)>(this, &MShowController::OnSpeakerTestClick));
			visual->EVENT_CLICK += m_onSpeakerTestClick;
			m_onSpeakerFocus.Reset(new Delegate<void(TinyVisual*, FocusEventArgs&)>(this, &MShowController::OnSpeakerFocus));
			visual->EVENT_FOCUS += m_onSpeakerFocus;
		}
		visual = m_window.GetDocument()->GetVisualByName("serverIP");
		if (visual != NULL && visual->IsKindOf(RUNTIME_CLASS(TinyVisualComboBox)))
		{
			TinyVisualComboBox* val = static_cast<TinyVisualComboBox*>(visual);
			vector<string> ips = m_appConfig.GetIPList();
			for (INT i = 0;i < ips.size();i++)
			{
				val->AddOption(ips[i].c_str(), ips[i].c_str());
			}
			val->SetSelected(0);
		}


		visual = m_window.GetDocument()->GetVisualByName("analyzeWND");
		if (visual != NULL && visual->IsKindOf(RUNTIME_CLASS(TinyVisualNative)))
		{
			TinyVisualNative* val = static_cast<TinyVisualNative*>(visual);
			val->SetWindow(&m_window.m_analyserBAR);
		}
		m_window.GetDocument()->Redraw();
	}
	void MShowController::Uninitialize()
	{
		m_bBreak = TRUE;
		m_task.Close(1000);
		m_audioDSP.Stop();
		m_audioDSP.Close();
		/*if (m_audioSDK != NULL)
		{
			m_audioSDK->release();
		}
		m_audioSDK.Reset(NULL);*/
		if (m_preview != NULL)
		{
			m_preview->Close();
			m_preview.Reset(NULL);
		}
	}

	MAppConfig& MShowController::AppConfig()
	{
		return m_appConfig;
	}
	CLSID MShowController::GetSpeakCLSID()
	{
		CLSID clsid = GUID_NULL;
		TinyVisual* visual = m_window.GetDocument()->GetVisualByName("speaker");
		if (visual != NULL && visual->IsKindOf(RUNTIME_CLASS(TinyVisualComboBox)))
		{
			TinyVisualComboBox* val = static_cast<TinyVisualComboBox*>(visual);
			TinyVisualOption* option = val->GetSelected();
			if (option != NULL)
			{
				wstring szGUID = option->GetValue().ToWString();
				CLSIDFromString(&szGUID[0], &clsid);
				return clsid;
			}
		}
		return clsid;
	}
	CLSID MShowController::GetMicrophoneCLSID()
	{
		CLSID clsid = GUID_NULL;
		TinyVisual* visual = m_window.GetDocument()->GetVisualByName("microphone");
		if (visual != NULL && visual->IsKindOf(RUNTIME_CLASS(TinyVisualComboBox)))
		{
			TinyVisualComboBox* val = static_cast<TinyVisualComboBox*>(visual);
			TinyVisualOption* option = val->GetSelected();
			if (option != NULL)
			{
				wstring szGUID = option->GetValue().ToWString();
				CLSIDFromString(&szGUID[0], &clsid);
				return clsid;
			}
		}
		return clsid;
	}
	void MShowController::OnMinimumClick(TinyVisual*, EventArgs& args)
	{
		m_window.GetDocument()->ReleaseCapture();//必须释放捕获
		SendMessage(m_window.Handle(), WM_SYSCOMMAND, SC_MINIMIZE, NULL);
	}

	void MShowController::OnCloseClick(TinyVisual*, EventArgs& args)
	{
		SendMessage(m_window.Handle(), WM_CLOSE, NULL, NULL);
	}

	void MShowController::OnMicrophoneTestClick(TinyVisual*, EventArgs& args)
	{
		TinyVisual* visual = m_window.GetDocument()->GetVisualByName("btnMicrophoneTest");
		if (visual != NULL)
		{
			m_window.GetDocument()->SetFocus(visual);
		}
		visual = m_window.GetDocument()->GetVisualByName("microphone");
		if (visual != NULL && visual->IsKindOf(RUNTIME_CLASS(TinyVisualComboBox)))
		{
			TinyVisualComboBox* val = static_cast<TinyVisualComboBox*>(visual);
			TinyVisualOption* option = val->GetSelected();
			if (option != NULL)
			{
				wstring szGUID = option->GetValue().ToWString();
				CLSID clsid;
				CLSIDFromString(&szGUID[0], &clsid);
				WAVEFORMATEX w;
				w.cbSize = 0;
				w.nChannels = 2;
				w.nSamplesPerSec = 44100;
				w.wBitsPerSample = 16;
				w.nBlockAlign = (w.wBitsPerSample * w.nChannels) / 8;
				w.nAvgBytesPerSec = w.nSamplesPerSec * w.nBlockAlign;
				w.wFormatTag = WAVE_FORMAT_PCM;
				m_microphoneTest.Invoke(clsid, val->GetDocument()->GetVisualHWND()->Handle());
			}
		}
	}

	void MShowController::OnSpeakerTestClick(TinyVisual*, EventArgs& args)
	{
		TinyVisual* visual = m_window.GetDocument()->GetVisualByName("btnSpeakerTest");
		if (visual != NULL)
		{
			m_window.GetDocument()->SetFocus(visual);
		}
		visual = m_window.GetDocument()->GetVisualByName("speaker");
		if (visual != NULL && visual->IsKindOf(RUNTIME_CLASS(TinyVisualComboBox)))
		{
			TinyVisualComboBox* val = static_cast<TinyVisualComboBox*>(visual);
			TinyVisualOption* option = val->GetSelected();
			if (option != NULL)
			{
				wstring szGUID = option->GetValue().ToWString();
				CLSID clsid;
				CLSIDFromString(&szGUID[0], &clsid);
				string szFile = StringPrintf("%s\%s", TinyVisualResource::GetInstance().GetDefaultPath().c_str(), "skin\\SoundTest.wav");
				m_speakTest.Invoke(szFile.c_str(), clsid, val->GetDocument()->GetVisualHWND()->Handle());
			}
		}
	}

	void MShowController::OnMicrophoneFocus(TinyVisual*, FocusEventArgs& args)
	{
		if (!args.IsFocus())
		{
			m_microphoneTest.Shutdown();
		}
	}

	void MShowController::OnSpeakerFocus(TinyVisual*, FocusEventArgs& args)
	{
		if (!args.IsFocus())
		{
			m_speakTest.Shutdown();
		}
	}

	void MShowController::OnPreviewClick(TinyVisual*, EventArgs& args)
	{
		TinyVisual* visual = m_window.GetDocument()->GetVisualByName("txtURL");
		if (visual != NULL && visual->IsKindOf(RUNTIME_CLASS(TinyVisualTextBox)))
		{
			TinyVisualTextBox* val = static_cast<TinyVisualTextBox*>(visual);
			TinyString szURL = val->GetText();
			if (m_preview != NULL && !szURL.IsEmpty())
			{
				m_preview->Close();
				if (m_preview->Open(szURL.CSTR()))
				{
					LOG(INFO) << "Preview Open OK" << endl;
				}
				else
				{
					LOG(ERROR) << "Preview Open Fail" << endl;
				}
			}
		}
	}

	void MShowController::OnRecordClick(TinyVisual*, EventArgs& args)
	{
		TinyVisual* visual = m_window.GetDocument()->GetVisualByName("serverIP");
		if (visual != NULL && visual->IsKindOf(RUNTIME_CLASS(TinyVisualComboBox)))
		{
			TinyVisualComboBox* val = static_cast<TinyVisualComboBox*>(visual);
			TinyVisualOption* option = val->GetSelected();
			if (option != NULL)
			{
				string ip = option->GetValue().CSTR();
				if (m_task.IsActive())
				{
					m_bBreak = TRUE;
					m_task.Close(1000);
				}
				m_bBreak = FALSE;
				if (m_task.Submit(BindCallback(&MShowController::OnMessagePump, this)))
				{
					m_audioSDK.Reset(new AudioSdk(ip, 6090));
					if (m_audioSDK != NULL)
					{
						if (m_audioSDK->init(44100, 2, 16) == 0)
						{
							LOG(INFO) << "AudioSDK init OK" << endl;
							CLSID speakerCLSID = GetSpeakCLSID();
							if (IsEqualGUID(speakerCLSID, GUID_NULL))
							{
								LOG(ERROR) << "GetSpeakCLSID is null" << endl;
							}
							CLSID microphoneCLSID = GetMicrophoneCLSID();
							if (IsEqualGUID(microphoneCLSID, GUID_NULL))
							{
								LOG(ERROR) << "GetMicrophoneCLSID is null" << endl;
							}
							m_audioDSP.Close();
							if (m_audioDSP.Open(microphoneCLSID, speakerCLSID))
							{
								m_audioDSP.Stop();
								m_audioDSP.Start();
								visual = m_window.GetDocument()->GetVisualByName("analyzeWND");
								visual->SetVisible(TRUE);
							}
						}
						else
						{
							LOG(ERROR) << "AudioSDK init Fail" << endl;
						}
					}
				}
			}
		}
	}
	void MShowController::OnMessagePump()
	{
		for (;;)
		{
			if (m_bBreak)
				break;
			if (m_event.Lock(1000))
			{
				if (m_audioSDK != NULL)
				{
					AUDIO_SAMPLE sample = { 0 };
					BOOL bRes = m_audioQueue.Pop(sample);
					if (bRes && sample.size > 0)
					{
						LOG(INFO) << "Timestamp: " << sample.timestamp;
						m_audioSDK->audio_encode_send(sample.bits + 4, static_cast<INT32>(sample.timestamp));
					}
					m_audioQueue.Free(sample.bits);
				}
			}
		}
	}
	void MShowController::OnAudio(BYTE* bits, LONG size, FLOAT ts, LPVOID)
	{
		if (size == 4096)
		{
			m_audioAnalyser.Process(m_window.m_analyserBAR.Handle(), bits, size);
			LONGLONG currentPTS = MShow::MShowApp::GetInstance().GetCurrentAudioTS() + m_preview->GetBasePTS();
			if (m_previousPTS != currentPTS)
			{
				m_previousPTS = currentPTS;
			}
			if (m_audioQueue.GetAllocSize() == 0)
			{
				INT count = 5;
				m_audioQueue.Initialize(count, size + 4);
			}
			AUDIO_SAMPLE sample = { 0 };
			sample.size = size;
			sample.bits = static_cast<BYTE*>(m_audioQueue.Alloc());
			sample.timestamp = currentPTS;
			memcpy(sample.bits + 4, bits, size);
			m_audioQueue.Push(sample);
			m_event.SetEvent();
		}
	}

	void MShowController::OnAudioDSP(BYTE* bits, LONG size)
	{
		if (size == 4096)
		{
			m_audioAnalyser.Process(m_window.m_analyserBAR.Handle(), bits, size);
			LONGLONG currentPTS = MShow::MShowApp::GetInstance().GetCurrentAudioTS() + m_preview->GetBasePTS();
			if (m_previousPTS != currentPTS)
			{
				m_previousPTS = currentPTS;
			}
			if (m_audioQueue.GetAllocSize() == 0)
			{
				INT count = 5;
				m_audioQueue.Initialize(count, size + 4);
			}
			AUDIO_SAMPLE sample = { 0 };
			sample.size = size;
			sample.bits = static_cast<BYTE*>(m_audioQueue.Alloc());
			sample.timestamp = currentPTS;
			memcpy(sample.bits + 4, bits, size);
			m_audioQueue.Push(sample);
			m_event.SetEvent();
		}
	}

	MPreviewController* MShowController::GetPreviewController()
	{
		return m_preview;
	}
}
