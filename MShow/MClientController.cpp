#include "stdafx.h"
#include "MClientController.h"
#include "MPreviewController.h"
#include "Network/TinyHTTPClient.h"
#include "MShow.h"
using namespace TinyUI::Network;
using namespace TinyUI::Windowless;

namespace MShow
{
	MClientController::MClientController(MClientWindow& view)
		:m_view(view),
		m_hTimer(NULL),
		m_bBreak(FALSE),
		m_previousPTS(0)
	{

	}

	MClientController::~MClientController()
	{
	}

	BOOL MClientController::Initialize()
	{
		m_preview.Reset(new MPreviewController(m_view.m_previewView));
		if (!m_preview)
			return FALSE;
		if (!m_preview->Initialize())
		{
			LOG(ERROR) << "Preview Initialize Fail";
			return FALSE;
		}
		if (!m_audioDSP.Initialize(BindCallback(&MClientController::OnAudioDSP, this)))
		{
			LOG(ERROR) << "AudioDSP Initialize Fail";
			return FALSE;
		}
		InitializeUI();
		return TRUE;
	}

	BOOL MClientController::Uninitialize()
	{
		Close();
		m_preview.Reset(NULL);
		return TRUE;
	}

	void MClientController::SetProgram(const string& szProgramName, const string& szProgramID, const string& szLogID)
	{
		m_szProgramID = std::move(szProgramID);
		m_szProgramName = std::move(szProgramName);
		m_szLogID = std::move(szLogID);
		TinyVisual* visual = m_view.GetDocument()->GetVisualByName("lblName");
		if (visual != NULL)
		{
			visual->SetText(szProgramName.c_str());
		}
		m_view.Invalidate();
	}
	void MClientController::SetTimes(const string& szTime1, const string& szTime2)
	{
		TinyVisual* visual = m_view.GetDocument()->GetVisualByName("lblBeginTime");
		if (visual != NULL)
		{
			visual->SetText(szTime1.c_str());
		}
		visual = m_view.GetDocument()->GetVisualByName("lblEndTime");
		if (visual != NULL)
		{
			visual->SetText(szTime2.c_str());
		}
		m_view.Invalidate();
	}
	BOOL MClientController::SetPreview(const string& szPreviewURL)
	{
		m_szPreviewURL = std::move(szPreviewURL);
		if (m_hTimer != NULL)
		{
			TinyApplication::GetInstance()->GetTimers().Unregister(m_hTimer);
			m_hTimer = NULL;
		}
		m_hTimer = TinyApplication::GetInstance()->GetTimers().Register(&MClientController::OnTimer, this, 1000, 0);
		return m_hTimer != NULL;
	}

	VOID CALLBACK MClientController::OnTimer(PVOID lpParam, BOOLEAN TimerOrWaitFired)
	{
		MClientController* pCTRL = static_cast<MClientController*>(lpParam);
		ASSERT(pCTRL);
		pCTRL->m_preview->Close();
		if (pCTRL->m_preview->Open(pCTRL->m_szPreviewURL.c_str()))
		{
			LOG(INFO) << "[SetPreview] " << "Open Preview :" << pCTRL->m_szPreviewURL << " OK";
		}
		else
		{
			TinyVisual* visual = pCTRL->m_view.GetDocument()->GetVisualByName("lblError");
			if (visual != NULL)
			{
				visual->SetVisible(TRUE);
				visual->SetText(StringPrintf("Open %s Error!", pCTRL->m_szPreviewURL.c_str()).c_str());
			}
			LOG(ERROR) << "[SetPreview] " << "Open Preview :" << pCTRL->m_szPreviewURL << " Fail";
		}
		pCTRL->m_view.Invalidate();
	}

	void MClientController::InitializeUI()
	{
		TinyVisual* visual = m_view.GetDocument()->GetVisualByName("lblError");
		if (visual != NULL)
		{
			visual->SetVisible(FALSE);
		}
		visual = m_view.GetDocument()->GetVisualByName("btnSetting");
		if (visual != NULL)
		{
			m_onSettingClick.Reset(new Delegate<void(TinyVisual*, EventArgs&)>(this, &MClientController::OnSettingClick));
			visual->EVENT_CLICK += m_onSettingClick;
		}
		visual = m_view.GetDocument()->GetVisualByName("sysmin");
		if (visual != NULL)
		{
			m_onMinimumClick.Reset(new Delegate<void(TinyVisual*, EventArgs&)>(this, &MClientController::OnMinimumClick));
			visual->EVENT_CLICK += m_onMinimumClick;
		}
		visual = m_view.GetDocument()->GetVisualByName("sysclose");
		if (visual != NULL)
		{
			m_onCloseClick.Reset(new Delegate<void(TinyVisual*, EventArgs&)>(this, &MClientController::OnCloseClick));
			visual->EVENT_CLICK += m_onCloseClick;
		}
		visual = m_view.GetDocument()->GetVisualByName("btnEdit");
		if (visual != NULL)
		{
			m_onEditClick.Reset(new Delegate<void(TinyVisual*, EventArgs&)>(this, &MClientController::OnEditClick));
			visual->EVENT_CLICK += m_onEditClick;
		}
		visual = m_view.GetDocument()->GetVisualByName("btnSave");
		if (visual != NULL)
		{
			m_onSaveClick.Reset(new Delegate<void(TinyVisual*, EventArgs&)>(this, &MClientController::OnSaveClick));
			visual->EVENT_CLICK += m_onSaveClick;
		}
		visual = m_view.GetDocument()->GetVisualByName("btnCancel");
		if (visual != NULL)
		{
			m_onCancelClick.Reset(new Delegate<void(TinyVisual*, EventArgs&)>(this, &MClientController::OnCancelClick));
			visual->EVENT_CLICK += m_onCancelClick;
		}
		visual = m_view.GetDocument()->GetVisualByName("btnCommentary");
		if (visual != NULL)
		{
			m_onCommentaryClick.Reset(new Delegate<void(TinyVisual*, EventArgs&)>(this, &MClientController::OnCommentaryClick));
			visual->EVENT_CLICK += m_onCommentaryClick;
		}
		visual = m_view.GetDocument()->GetVisualByName("previewWND");
		if (visual != NULL && visual->IsKindOf(RUNTIME_CLASS(TinyVisualNative)))
		{
			TinyVisualNative* val = static_cast<TinyVisualNative*>(visual);
			val->SetWindow(&m_view.m_previewView);
		}
		vector<CAPTUREDEVICE> captures;
		TinySoundCapture::Enumerate(captures);
		visual = m_view.GetDocument()->GetVisualByName("microphone");
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
		visual = m_view.GetDocument()->GetVisualByName("speaker");
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
		visual = m_view.GetDocument()->GetVisualByName("btnMicrophoneTest");
		if (visual != NULL)
		{
			m_onMicrophoneTestClick.Reset(new Delegate<void(TinyVisual*, EventArgs&)>(this, &MClientController::OnMicrophoneTestClick));
			visual->EVENT_CLICK += m_onMicrophoneTestClick;
			m_onMicrophoneFocus.Reset(new Delegate<void(TinyVisual*, FocusEventArgs&)>(this, &MClientController::OnMicrophoneFocus));
			visual->EVENT_FOCUS += m_onMicrophoneFocus;
		}
		visual = m_view.GetDocument()->GetVisualByName("btnSpeakerTest");
		if (visual != NULL)
		{
			m_onSpeakerTestClick.Reset(new Delegate<void(TinyVisual*, EventArgs&)>(this, &MClientController::OnSpeakerTestClick));
			visual->EVENT_CLICK += m_onSpeakerTestClick;
			m_onSpeakerFocus.Reset(new Delegate<void(TinyVisual*, FocusEventArgs&)>(this, &MClientController::OnSpeakerFocus));
			visual->EVENT_FOCUS += m_onSpeakerFocus;
		}
	}

	void MClientController::OnMinimumClick(TinyVisual*, EventArgs& args)
	{
		m_view.GetDocument()->ReleaseCapture();//±ØÐëÊÍ·Å²¶»ñ
		SendMessage(m_view.Handle(), WM_SYSCOMMAND, SC_MINIMIZE, NULL);
	}

	void MClientController::OnCloseClick(TinyVisual*, EventArgs& args)
	{
		if (this->Remove(m_szSourceID))
		{
			m_szSourceID.clear();
		}
		this->Close();
		m_view.ShowWindow(SW_HIDE);
		m_view.UpdateWindow();
		MShow::MShowApp::GetInstance().GetSearchView().ShowWindow(SW_NORMAL);
		MShow::MShowApp::GetInstance().GetSearchView().UpdateWindow();
	}

	void MClientController::OnSettingClick(TinyVisual*, EventArgs& args)
	{
		TinyVisual* visual = m_view.GetDocument()->GetVisualByName("setting");
		if (visual != NULL)
		{
			visual->SetVisible(!visual->IsVisible());
			m_view.Invalidate();
		}
	}

	void MClientController::OnEditClick(TinyVisual*, EventArgs& args)
	{
		TinyVisual* visual = m_view.GetDocument()->GetVisualByName("btnEdit");
		if (visual != NULL)
		{
			visual->SetVisible(FALSE);
		}
		visual = m_view.GetDocument()->GetVisualByName("btnSave");
		if (visual != NULL)
		{
			visual->SetVisible(TRUE);
			m_view.GetDocument()->SetFocus(visual);
		}
		visual = m_view.GetDocument()->GetVisualByName("btnCancel");
		if (visual != NULL)
		{
			visual->SetVisible(TRUE);
		}
		TinyVisualTextBox* pTextBox = static_cast<TinyVisualTextBox*>(m_view.GetDocument()->GetVisualByName("txtName"));
		if (pTextBox != NULL)
		{
			pTextBox->SetEnable(TRUE);
		}
		m_view.Invalidate();
	}
	void MClientController::OnSaveClick(TinyVisual*, EventArgs& args)
	{
		TinyVisual* visual = m_view.GetDocument()->GetVisualByName("btnEdit");
		if (visual != NULL)
		{
			visual->SetVisible(TRUE);
			m_view.GetDocument()->SetFocus(visual);
		}
		visual = m_view.GetDocument()->GetVisualByName("btnSave");
		if (visual != NULL)
		{
			visual->SetVisible(FALSE);
		}
		visual = m_view.GetDocument()->GetVisualByName("btnCancel");
		if (visual != NULL)
		{
			visual->SetVisible(FALSE);
		}
		TinyVisualTextBox* pTextBox = static_cast<TinyVisualTextBox*>(m_view.GetDocument()->GetVisualByName("txtName"));
		if (pTextBox != NULL)
		{
			m_szName = pTextBox->GetText().CSTR();
			pTextBox->SetEnable(FALSE);
		}
		m_view.Invalidate();
	}
	void MClientController::OnCancelClick(TinyVisual*, EventArgs& args)
	{
		TinyVisual* visual = m_view.GetDocument()->GetVisualByName("btnEdit");
		if (visual != NULL)
		{
			visual->SetVisible(TRUE);
			m_view.GetDocument()->SetFocus(visual);
		}
		visual = m_view.GetDocument()->GetVisualByName("btnSave");
		if (visual != NULL)
		{
			visual->SetVisible(FALSE);
		}
		visual = m_view.GetDocument()->GetVisualByName("btnCancel");
		if (visual != NULL)
		{
			visual->SetVisible(FALSE);
		}
		TinyVisualTextBox* pTextBox = static_cast<TinyVisualTextBox*>(m_view.GetDocument()->GetVisualByName("txtName"));
		if (pTextBox != NULL)
		{
			pTextBox->SetText(m_szName.c_str());
			pTextBox->SetEnable(FALSE);
		}
		m_view.Invalidate();
	}

	BOOL MClientController::Add()
	{
		TinyVisualTextBox* pTextBox = static_cast<TinyVisualTextBox*>(m_view.GetDocument()->GetVisualByName("txtName"));
		ASSERT(pTextBox);
		string code;
		string context;
		Json::Reader reader;
		Json::Value value;
		Json::Value result;
		TinyHTTPClient client;
		client.GetRequest().SetVerbs(TinyHTTPClient::POST);
		client.GetRequest().Add(TinyHTTPClient::ContentType, "application/x-www-form-urlencoded");
		client.GetRequest().Add("Sign", "#f93Uc31K24()_@");
		TinyString szName = pTextBox->GetText();
		string body = StringPrintf("name=%s&programId=%s&directorId=%s&streamUrl=", szName.CSTR(), m_szProgramID.c_str(), m_szLogID.c_str());
		body = std::move(ASCIIToUTF8(body));
		client.GetRequest().SetBody(body);
		string address = StringPrintf("%s/%s", MShow::MShowApp::GetInstance().AppConfig().GetPrefix().c_str(), "commentary/add");
		if (!client.Open(address))
		{
			LOG(ERROR) << "[MClientController] " << "Open " << address << " Fail";
			goto _ERROR;
		}
		if (!client.GetResponse().ReadAsString(context))
		{
			LOG(ERROR) << "[MClientController] " << "Read Json Fail";
			goto _ERROR;
		}
		if (!reader.parse(context, value))
		{
			LOG(ERROR) << "[MClientController] " << "Parse Json Fail";
			goto _ERROR;
		}
		code = value["code"].asString();
		if (code == "A00000")
		{
			m_szSourceID = std::to_string(value["data"].asInt());
			LOG(INFO) << "[MClientController] " << "Add SourceID :" << m_szSourceID << " OK";
			return TRUE;
		}
		else
		{
			string msg = value["msg"].asString();
			LOG(ERROR) << "[MClientController] " << "Response Code : " << code << " Msg: " << msg;
		}
	_ERROR:
		return FALSE;
	}
	BOOL MClientController::Remove(const string& sourceID)
	{
		if (sourceID.empty())
			return FALSE;
		string code;
		string context;
		Json::Reader reader;
		Json::Value value;
		Json::Value result;
		TinyHTTPClient client;
		client.GetRequest().SetVerbs(TinyHTTPClient::POST);
		client.GetRequest().Add(TinyHTTPClient::ContentType, "application/x-www-form-urlencoded");
		client.GetRequest().Add("Sign", "#f93Uc31K24()_@");
		string body = StringPrintf("id=%s&directorId=%s", sourceID.c_str(), m_szLogID.c_str());
		body = std::move(ASCIIToUTF8(body));
		client.GetRequest().SetBody(body);
		string address = StringPrintf("%s/%s", MShow::MShowApp::GetInstance().AppConfig().GetPrefix().c_str(), "commentary/del");
		if (!client.Open(address))
		{
			LOG(ERROR) << "[MClientController] " << "Open " << address << " Fail";
			goto _ERROR;
		}
		if (!client.GetResponse().ReadAsString(context))
		{
			LOG(ERROR) << "[MClientController] " << "Read Json Fail";
			goto _ERROR;
		}
		if (!reader.parse(context, value))
		{
			LOG(ERROR) << "[MClientController] " << "Parse Json Fail";
			goto _ERROR;
		}
		code = value["code"].asString();
		if (code == "A00000")
		{
			LOG(INFO) << "[MClientController] " << "Remove SourceID :" << m_szSourceID << " OK";
			return TRUE;
		}
		else
		{
			string msg = value["msg"].asString();
			msg = std::move(UTF8ToASCII(msg));
			LOG(ERROR) << "[MClientController] " << "Response Code : " << code << " Msg: " << msg;
		}
	_ERROR:
		return FALSE;
	}

	void MClientController::Close()
	{
		if (m_hTimer != NULL)
		{
			TinyApplication::GetInstance()->GetTimers().Unregister(m_hTimer);
			m_hTimer = NULL;
		}
		m_audioDSP.Stop();
		m_audioDSP.Close();
		if (m_preview != NULL)
		{
			m_preview->Close();
		}
	}

	void MClientController::OnCommentaryClick(TinyVisual*, EventArgs& args)
	{
		Add();
	}

	void MClientController::OnMicrophoneTestClick(TinyVisual*, EventArgs& args)
	{
		TinyVisual* visual = m_view.GetDocument()->GetVisualByName("btnMicrophoneTest");
		if (visual != NULL)
		{
			m_view.GetDocument()->SetFocus(visual);
		}
		visual = m_view.GetDocument()->GetVisualByName("microphone");
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

	void MClientController::OnSpeakerTestClick(TinyVisual*, EventArgs& args)
	{
		TinyVisual* visual = m_view.GetDocument()->GetVisualByName("btnSpeakerTest");
		if (visual != NULL)
		{
			m_view.GetDocument()->SetFocus(visual);
		}
		visual = m_view.GetDocument()->GetVisualByName("speaker");
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

	void MClientController::OnMicrophoneFocus(TinyVisual*, FocusEventArgs& args)
	{
		if (!args.IsFocus())
		{
			m_microphoneTest.Shutdown();
		}
	}

	void MClientController::OnSpeakerFocus(TinyVisual*, FocusEventArgs& args)
	{
		if (!args.IsFocus())
		{
			m_speakTest.Shutdown();
		}
	}

	void MClientController::OnAudioDSP(BYTE* bits, LONG size)
	{
		if (size == 4096)
		{
			//m_audioAnalyser.Process(m_window.m_analyserBAR.Handle(), bits, size);
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
			//m_event.SetEvent();
		}
	}

	void MClientController::OnMessagePump()
	{

	}
}