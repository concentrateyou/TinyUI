#include "stdafx.h"
#include "MClientController.h"
#include "MPreviewController.h"
#include "Network/TinyHTTPClient.h"
#include "MShow.h"
#include "MAudioDB.h"
using namespace TinyUI::Network;
using namespace TinyUI::Windowless;

namespace MShow
{
	MClientController::MClientController(MClientWindow& view)
		:m_view(view),
		m_bBreak(FALSE),
		m_bBreak1(FALSE),
		m_bCommentarying(FALSE),
		m_bPause(FALSE),
		m_previousPTS(-1)
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
		if (!m_audioDSP.Initialize(BindCallback(&MClientController::OnAudio, this)))
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
		visual = m_view.GetDocument()->GetVisualByName("lblError");
		if (visual != NULL)
		{
			visual->SetVisible(FALSE);
			visual->SetText("");
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
		if (m_preview != NULL)
		{
			m_preview->Close();
			if (m_preview->Open(m_szPreviewURL.c_str()))
			{
				LOG(INFO) << "[SetPreview] " << "Open Preview :" << m_szPreviewURL << " OK";
				TinyVisual* visual = m_view.GetDocument()->GetVisualByName("btnStartCommentary");
				if (visual != NULL)
				{
					visual->SetVisible(TRUE);
				}
			}
			else
			{
				TinyVisual* visual = m_view.GetDocument()->GetVisualByName("btnStartCommentary");
				if (visual != NULL)
				{
					visual->SetVisible(FALSE);
				}
				visual = m_view.GetDocument()->GetVisualByName("lblError");
				if (visual != NULL)
				{
					visual->SetVisible(TRUE);
					visual->SetText("Ԥ������ʧ�� ��������.....");
				}
				LOG(ERROR) << "[SetPreview] " << "Open Preview :" << m_szPreviewURL << " Fail";
				m_taskTimer.SetCallback(3000, BindCallback(&MClientController::OnTry, this));//ÿ��3������
			}
		}
		m_view.Invalidate();
		return TRUE;
	}
	void MClientController::OnTry()
	{
		if (m_preview != NULL)
		{
			BOOL bRes = m_preview->Close();
			TRACE("[MClientController] OnTry Close:%d\n", bRes);
			LOG(INFO) << "[MClientController] OnTry Close:" << bRes << "\n\n\n\n\n";
			if (m_preview->Open(m_szPreviewURL.c_str()))
			{
				TRACE("[MClientController] OnTry Open OK\n");
				LOG(INFO) << "[MClientController] OnTry Open OK";
				m_taskTimer.Close();
				TinyVisual* visual = m_view.GetDocument()->GetVisualByName("btnStartCommentary");
				if (visual != NULL)
				{
					visual->SetVisible(TRUE);
				}
				visual = m_view.GetDocument()->GetVisualByName("lblError");
				if (visual != NULL)
				{
					visual->SetVisible(FALSE);
					visual->SetText("");
				}
				m_view.Invalidate();
			}
			else
			{
				TRACE("[MClientController] OnTry Open FAIL\n");
				LOG(ERROR) << "[MClientController] OnTry Open FAIL";
			}
		}
	}
	void MClientController::UpdateMicrophones()
	{
		vector<CAPTUREDEVICE> captures;
		TinySoundCapture::Enumerate(captures);
		TinyVisual* visual = m_view.GetDocument()->GetVisualByName("microphone");
		if (visual != NULL && visual->IsKindOf(RUNTIME_CLASS(TinyVisualComboBox)))
		{
			TinyVisualComboBox* val = static_cast<TinyVisualComboBox*>(visual);
			val->RemoveAll();
			for (size_t i = 0;i < captures.size();i++)
			{
				wstring szGUID;
				szGUID.resize(39);
				::StringFromGUID2(captures[i].Guid, &szGUID[0], 39);
				string sz = WStringToString(szGUID);
				val->AddOption(sz.c_str(), captures[i].Description.c_str());
			}
			val->SetSelected(0);
		}
	}
	void MClientController::UpdateSpeakers()
	{
		vector<PLAYDEVICE> renders;
		TinySoundPlayer::Enumerate(renders);
		TinyVisual* visual = m_view.GetDocument()->GetVisualByName("speaker");
		if (visual != NULL && visual->IsKindOf(RUNTIME_CLASS(TinyVisualComboBox)))
		{
			TinyVisualComboBox* val = static_cast<TinyVisualComboBox*>(visual);
			val->RemoveAll();
			for (size_t i = 0;i < renders.size();i++)
			{
				wstring szGUID;
				szGUID.resize(39);
				::StringFromGUID2(renders[i].Guid, &szGUID[0], 39);
				string sz = WStringToString(szGUID);
				val->AddOption(sz.c_str(), renders[i].Description.c_str());
			}
			val->SetSelected(0);
		}
	}

	void MClientController::OnTimerStatus()
	{
		INT count = 0;
		if (this->Query(m_szSourceID, count) && count == 0)
		{
			//ֹͣTimer
			m_timerStatus.Close();
			Disconnect(m_szSourceID);
			StopCommentary();
			Close();
			if (MessageBox(m_view.Handle(), "��ǰ�ͻ������ӱ��Ƴ�", "����", MB_OK) == IDOK)
			{
				m_view.ShowWindow(SW_HIDE);
				m_view.UpdateWindow();
				MShow::MShowApp::GetInstance().GetSearchController().Refresh();
				MShow::MShowApp::GetInstance().GetSearchView().ShowWindow(SW_NORMAL);
				MShow::MShowApp::GetInstance().GetSearchView().UpdateWindow();
			}
		}
	}

	BOOL MClientController::Query(const string& sourceID, INT& count)
	{
		string code;
		string context;
		Json::Reader reader;
		Json::Value value;
		Json::Value result;
		TinyHTTPClient client;
		client.SetTimeout(3000);
		client.GetRequest().SetVerbs(TinyHTTPClient::GET);
		client.GetRequest().Add(TinyHTTPClient::ContentType, "application/x-www-form-urlencoded");
		client.GetRequest().Add("Sign", "#f93Uc31K24()_@");
		string address;
		if (sourceID.empty())
		{
			address = StringPrintf("%s/%s?programId=%s&directorId=%s", MShow::MShowApp::GetInstance().AppConfig().GetPrefix().c_str(), "commentary/list", m_szProgramID.c_str(), m_szLogID.c_str());
		}
		else
		{
			address = StringPrintf("%s/%s?id=%s&programId=%s&directorId=%s", MShow::MShowApp::GetInstance().AppConfig().GetPrefix().c_str(), "commentary/list", sourceID.c_str(), m_szProgramID.c_str(), m_szLogID.c_str());
		}
		if (!client.Open(address))
		{
			LOG(ERROR) << "[MClientController] " << "Open " << address << " " << client.GetResponse().GetGetStatusMsg();
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
			count = value["data"].size();
			//TRACE("List SourceID:%s   OK\n", m_szSourceID.c_str());
			LOG(INFO) << "[MClientController] " << "List SourceID :" << m_szSourceID << " OK";
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
		visual = m_view.GetDocument()->GetVisualByName("btnStartCommentary");
		if (visual != NULL)
		{
			m_onStartCommentaryClick.Reset(new Delegate<void(TinyVisual*, EventArgs&)>(this, &MClientController::OnStartCommentaryClick));
			visual->EVENT_CLICK += m_onStartCommentaryClick;
		}
		visual = m_view.GetDocument()->GetVisualByName("btnPauseCommentary");
		if (visual != NULL)
		{
			m_onPauseCommentaryClick.Reset(new Delegate<void(TinyVisual*, EventArgs&)>(this, &MClientController::OnPauseCommentaryClick));
			visual->EVENT_CLICK += m_onPauseCommentaryClick;
		}
		visual = m_view.GetDocument()->GetVisualByName("btnStopCommentary");
		if (visual != NULL)
		{
			m_onStopCommentaryClick.Reset(new Delegate<void(TinyVisual*, EventArgs&)>(this, &MClientController::OnStopCommentaryClick));
			visual->EVENT_CLICK += m_onStopCommentaryClick;
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
			for (size_t i = 0;i < captures.size();i++)
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
			for (size_t i = 0;i < renders.size();i++)
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
		m_view.Invalidate();
	}

	void MClientController::OnMinimumClick(TinyVisual*, EventArgs& args)
	{
		m_view.GetDocument()->ReleaseCapture();//�����ͷŲ���
		SendMessage(m_view.Handle(), WM_SYSCOMMAND, SC_MINIMIZE, NULL);
	}

	void MClientController::OnCloseClick(TinyVisual*, EventArgs& args)
	{
		this->Disconnect(m_szSourceID);
		this->Close();
		m_view.ShowWindow(SW_HIDE);
		m_view.UpdateWindow();
		MShow::MShowApp::GetInstance().GetSearchController().Refresh();
		MShow::MShowApp::GetInstance().GetSearchView().ShowWindow(SW_NORMAL);
		MShow::MShowApp::GetInstance().GetSearchView().UpdateWindow();
		LOG(INFO) << "OnCloseClick OK";
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
		TinyVisualTextBox* pTextBox = static_cast<TinyVisualTextBox*>(m_view.GetDocument()->GetVisualByName("txtName"));
		if (pTextBox != NULL)
		{
			if (pTextBox->GetText().IsEmpty())
			{
				MessageBox(m_view.Handle(), "��˵������Ϊ��!", "��ʾ", MB_OK);
				return;
			}
			m_szName = pTextBox->GetText().CSTR();
			pTextBox->SetEnable(FALSE);
		}
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
		m_view.Invalidate();
		UpdateName(m_szSourceID, m_szName);
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

	BOOL MClientController::Connect()
	{
		TinyVisualTextBox* pTextBox = static_cast<TinyVisualTextBox*>(m_view.GetDocument()->GetVisualByName("txtName"));
		ASSERT(pTextBox);
		string code;
		string context;
		Json::Reader reader;
		Json::Value value;
		Json::Value result;
		TinyHTTPClient client;
		client.SetTimeout(3000);
		client.GetRequest().SetVerbs(TinyHTTPClient::POST);
		client.GetRequest().Add(TinyHTTPClient::ContentType, "application/x-www-form-urlencoded");
		client.GetRequest().Add("Sign", "#f93Uc31K24()_@");
		TinyString szName = pTextBox->GetText();
		string body;
		if (szName.GetSize() == 0)
		{
			body = StringPrintf("programId=%s&directorId=%s", m_szProgramID.c_str(), m_szLogID.c_str());
		}
		else
		{
			body = StringPrintf("name=%s&programId=%s&directorId=%s", szName.CSTR(), m_szProgramID.c_str(), m_szLogID.c_str());
		}
		body = std::move(ASCIIToUTF8(body));
		client.GetRequest().SetBody(body);
		string address = StringPrintf("%s/%s", MShow::MShowApp::GetInstance().AppConfig().GetPrefix().c_str(), "commentary/add");
		if (!client.Open(address))
		{
			LOG(ERROR) << "[MClientController] " << "Open " << address << " " << client.GetResponse().GetGetStatusMsg();
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
			if (m_szName.empty())
			{
				m_szName = StringPrintf("��˵�ź�Դ%s", m_szSourceID.c_str());
			}
			TinyVisualTextBox* pTextBox = static_cast<TinyVisualTextBox*>(m_view.GetDocument()->GetVisualByName("txtName"));
			if (pTextBox != NULL)
			{
				pTextBox->SetText(m_szName.c_str());
			}
			LOG(INFO) << "[MClientController] " << "Add SourceID :" << m_szSourceID << " OK";
			TRACE("Add SourceID:%s   OK\n", m_szSourceID.c_str());
			return TRUE;
		}
		else
		{
			string msg = value["msg"].asString();
			msg = std::move(UTF8ToASCII(msg));
			MessageBox(m_view.Handle(), msg.c_str(), "��ʾ!", MB_OK);
			LOG(ERROR) << "[MClientController] " << "Response Code : " << code << " Msg: " << msg;
		}
	_ERROR:
		return FALSE;

	}

	BOOL MClientController::GetPreviewURL(string& szURL, INT& iAudio, string& szIP)
	{
		string code;
		string context;
		Json::Reader reader;
		Json::Value value;
		Json::Value result;
		TinyHTTPClient client;
		client.SetTimeout(3000);
		client.GetRequest().SetVerbs(TinyHTTPClient::GET);
		string address = StringPrintf("http://%s:8001/querycommentaryPURL?PID=%s&ID=%s", MShow::MShowApp::GetInstance().AppConfig().GetDispatch().c_str(), m_szProgramID.c_str(), m_szSourceID.c_str());
		if (!client.Open(address))
		{
			LOG(ERROR) << "[MClientController] " << "Open " << address << " " << client.GetResponse().GetGetStatusMsg();
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
		code = value["error_code"].asString();
		if (code == "A00000")
		{
			szURL = value["result"].asString();
			vector<string> vals;
			SplitString(szURL, ';', &vals);
			if (vals.size() == 3)
			{
				szURL = vals[0];
				iAudio = std::stoi(vals[1]);
				szIP = vals[2];
				TRACE("URL :%s, PORT:%d\n", szURL.c_str(), iAudio);
				LOG(INFO) << "[MClientController] " << "GetPreviewURL :" << vals[0] << " PORT: " << iAudio << " IP:" << szIP << " OK";
				return TRUE;
			}
		}
		else
		{
			string msg = value["error_msg"].asString();
			msg = std::move(UTF8ToASCII(msg));
			LOG(ERROR) << "[MClientController] " << "Response Code : " << code << " Msg: " << msg;
		}
		return TRUE;
	_ERROR:
		return FALSE;
	}

	BOOL MClientController::Disconnect(const string& sourceID, BOOL del)
	{
		if (sourceID.empty())
			return FALSE;
		string code;
		string context;
		string body;
		string address;
		Json::Reader reader;
		Json::Value value;
		Json::Value result;
		TinyHTTPClient client;
		client.SetTimeout(3000);
		client.GetRequest().SetVerbs(TinyHTTPClient::POST);
		client.GetRequest().Add(TinyHTTPClient::ContentType, "application/x-www-form-urlencoded");
		client.GetRequest().Add("Sign", "#f93Uc31K24()_@");
		if (del)
		{
			body = StringPrintf("id=%s&directorId=%s", sourceID.c_str(), m_szLogID.c_str());//�Ͽ�
			address = StringPrintf("%s/%s", MShow::MShowApp::GetInstance().AppConfig().GetPrefix().c_str(), "commentary/del");
		}
		else
		{
			body = StringPrintf("id=%s&programId=%s&directorId=%s&status=2", sourceID.c_str(), m_szProgramID.c_str(), m_szLogID.c_str());//�Ͽ�
			address = StringPrintf("%s/%s", MShow::MShowApp::GetInstance().AppConfig().GetPrefix().c_str(), "commentary/edit");
		}
		body = std::move(ASCIIToUTF8(body));
		client.GetRequest().SetBody(body);
		if (!client.Open(address))
		{
			LOG(ERROR) << "[MClientController] " << "Open " << address << " " << client.GetResponse().GetGetStatusMsg();
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
			//TRACE("Disconnect SourceID:%s   OK\n", m_szSourceID.c_str());
			LOG(INFO) << "[MClientController] " << "Disconnect SourceID :" << m_szSourceID << " OK";
			return TRUE;
		}
		else
		{
			string msg = value["msg"].asString();
			if (!msg.empty())
				msg = std::move(UTF8ToASCII(msg));
			LOG(ERROR) << "[MClientController] " << "Response Code : " << code << " Msg: " << msg;
		}
	_ERROR:
		return FALSE;
	}
	BOOL MClientController::UpdateName(const string& sourceID, const string& sName)
	{
		if (sourceID.empty() || sName.empty())
			return FALSE;
		string code;
		string context;
		Json::Reader reader;
		Json::Value value;
		Json::Value result;
		TinyHTTPClient client;
		client.SetTimeout(3000);
		client.GetRequest().SetVerbs(TinyHTTPClient::POST);
		client.GetRequest().Add(TinyHTTPClient::ContentType, "application/x-www-form-urlencoded");
		client.GetRequest().Add("Sign", "#f93Uc31K24()_@");
		string body = StringPrintf("id=%s&programId=%s&directorId=%s&name=%s", sourceID.c_str(), m_szProgramID.c_str(), m_szLogID.c_str(), sName.c_str());
		body = std::move(ASCIIToUTF8(body));
		client.GetRequest().SetBody(body);
		string address = StringPrintf("%s/%s", MShow::MShowApp::GetInstance().AppConfig().GetPrefix().c_str(), "commentary/edit");
		if (!client.Open(address))
		{
			LOG(ERROR) << "[MClientController] " << "Open " << address << " " << client.GetResponse().GetGetStatusMsg();
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
			TRACE("Update SourceID:%s   OK\n", m_szSourceID.c_str());
			LOG(INFO) << "[MClientController] " << "Update SourceID :" << m_szSourceID << " OK";
			return TRUE;
		}
		else
		{
			string msg = value["msg"].asString();
			if (!msg.empty())
				msg = std::move(UTF8ToASCII(msg));
			LOG(ERROR) << "[MClientController] " << "Response Code : " << code << " Msg: " << msg;
		}
	_ERROR:
		return FALSE;
	}
	BOOL MClientController::UpdatePreviewURL(const string& sourceID, const string& sURL)
	{
		if (sourceID.empty() || sURL.empty())
			return FALSE;
		string code;
		string context;
		Json::Reader reader;
		Json::Value value;
		Json::Value result;
		TinyHTTPClient client;
		client.SetTimeout(3000);
		client.GetRequest().SetVerbs(TinyHTTPClient::POST);
		client.GetRequest().Add(TinyHTTPClient::ContentType, "application/x-www-form-urlencoded");
		client.GetRequest().Add("Sign", "#f93Uc31K24()_@");
		string body = StringPrintf("id=%s&programId=%s&directorId=%s&streamUrl=%s", sourceID.c_str(), m_szProgramID.c_str(), m_szLogID.c_str(), sURL.c_str());
		body = std::move(ASCIIToUTF8(body));
		client.GetRequest().SetBody(body);
		string address = StringPrintf("%s/%s", MShow::MShowApp::GetInstance().AppConfig().GetPrefix().c_str(), "commentary/edit");
		if (!client.Open(address))
		{
			LOG(ERROR) << "[MClientController] " << "Open " << address << " " << client.GetResponse().GetGetStatusMsg();
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
			TRACE("Update SourceID:%s   OK\n", m_szSourceID.c_str());
			LOG(INFO) << "[MClientController] " << "Update SourceID :" << m_szSourceID << " OK";
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
		if (m_view.GetDocument() != NULL)
		{
			//����UI 
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
				pTextBox->SetText("");
				pTextBox->SetEnable(FALSE);
			}
		}
		//ֹͣ��˵
		StopCommentary();
		//�ر�Ԥ��
		if (m_preview != NULL)
		{
			m_preview->Close();
		}
		m_taskTimer.Close();
		m_szSourceID.clear();
		m_szProgramID.clear();
		m_szName.clear();
		m_szLogID.clear();
	}
	BOOL MClientController::StartCommentary()
	{
		TinyPerformanceTime timeQPC;
		m_bBreak = FALSE;
		timeQPC.BeginTime();
		//��ȡ��ƵԤ����
		string szIP;
		INT iAudio = 0;
		if (!GetPreviewURL(m_szURL, iAudio, szIP) || m_szURL.empty())
		{
			goto _ERROR;
		}
		timeQPC.EndTime();
		LOG(INFO) << "[MClientController] GetPreviewURL Cost:" << timeQPC.GetMillisconds();
		timeQPC.BeginTime();
		m_audioSDK.Reset(new AudioSdk(szIP, iAudio, std::stoi(m_szSourceID)));
		if (m_audioSDK != NULL)
		{
			if (m_audioSDK->init(44100, 2, 16) != 0)
			{
				timeQPC.EndTime();
				LOG(INFO) << "[MClientController] AudioSDK Cost:" << timeQPC.GetMillisconds();
				LOG(ERROR) << "AudioSDK init Fail" << endl;
				goto _ERROR;
			}
			timeQPC.EndTime();
			LOG(INFO) << "[MClientController] AudioSDK Cost:" << timeQPC.GetMillisconds();
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
			timeQPC.BeginTime();
			m_audioDSP.Close();
			if (m_audioDSP.Open(microphoneCLSID, speakerCLSID))
			{
				m_audioDSP.Stop();
				m_audioDSP.Start();
			}
			timeQPC.EndTime();
			LOG(INFO) << "[MClientController] AudioDSP Cost:" << timeQPC.GetMillisconds();
		}
		timeQPC.BeginTime();
		//����SDK��������
		if (m_task.IsActive())
		{
			m_bBreak = TRUE;
			m_task.Close(INFINITE);
		}
		timeQPC.EndTime();
		LOG(INFO) << "[MClientController] AudioSDK Task Close Cost:" << timeQPC.GetMillisconds();
		if (!m_task.Submit(BindCallback(&MClientController::OnMessagePump, this)))
		{
			goto _ERROR;
		}
		timeQPC.BeginTime();
		//֪ͨWeb����
		if (!UpdatePreviewURL(m_szSourceID, m_szURL))
		{
			goto _ERROR;
		}
		timeQPC.EndTime();
		LOG(INFO) << "[MClientController] UpdatePreviewURL Cost:" << timeQPC.GetMillisconds();
		//��ʼTimer
		if (!m_timerStatus.SetCallback(1000, BindCallback(&MClientController::OnTimerStatus, this)))
		{
			goto _ERROR;
		}
		//����UI
		m_bCommentarying = TRUE;
		TinyVisual* spvis = m_view.GetDocument()->GetVisualByName("btnStartCommentary");
		if (spvis != NULL)
		{
			spvis->SetVisible(FALSE);
		}
		spvis = m_view.GetDocument()->GetVisualByName("btnPauseCommentary");
		if (spvis != NULL)
		{
			spvis->SetVisible(TRUE);
		}
		spvis = m_view.GetDocument()->GetVisualByName("btnStopCommentary");
		if (spvis != NULL)
		{
			spvis->SetVisible(TRUE);
		}
		m_view.Invalidate();
		return TRUE;
	_ERROR:
		StopCommentary();
		Disconnect(m_szSourceID, TRUE);
		return FALSE;
	}
	void MClientController::StopCommentary()
	{
		//ֹͣ�����߳�
		if (m_task.IsActive())
		{
			m_bBreak = TRUE;
			m_task.Close(INFINITE);
		}
		//ֹͣ�ɼ�
		m_audioDSP.Close();
		//�ͷ�SDK
		if (m_audioSDK != NULL)
		{
			m_audioSDK->release();
		}
		m_audioSDK.Reset(NULL);
		//ֹͣTimer
		m_timerStatus.Close();
		//����UI
		m_bPause = FALSE;
		m_szName.clear();
		m_szSourceID.clear();
		m_szURL.clear();
		if (m_view.GetDocument() != NULL)
		{
			TinyVisual* spvis = m_view.GetDocument()->GetVisualByName("btnPauseCommentary");
			if (spvis != NULL)
			{
				spvis->SetText(m_bPause ? "����" : "��ͣ");
			}
			m_bCommentarying = FALSE;
			spvis = m_view.GetDocument()->GetVisualByName("btnStartCommentary");
			if (spvis != NULL)
			{
				spvis->SetVisible(TRUE);
			}
			spvis = m_view.GetDocument()->GetVisualByName("btnPauseCommentary");
			if (spvis != NULL)
			{
				spvis->SetVisible(FALSE);
			}
			spvis = m_view.GetDocument()->GetVisualByName("btnStopCommentary");
			if (spvis != NULL)
			{
				spvis->SetVisible(FALSE);
			}
			TinyVisualTextBox* pTextBox = static_cast<TinyVisualTextBox*>(m_view.GetDocument()->GetVisualByName("txtName"));
			if (pTextBox != NULL)
			{
				pTextBox->SetEnable(FALSE);
				pTextBox->SetText("");
			}
		}
		m_view.Invalidate();
	}

	CLSID MClientController::GetSpeakCLSID()
	{
		CLSID clsid = GUID_NULL;
		TinyVisual* visual = m_view.GetDocument()->GetVisualByName("speaker");
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
	CLSID MClientController::GetMicrophoneCLSID()
	{
		CLSID clsid = GUID_NULL;
		TinyVisual* visual = m_view.GetDocument()->GetVisualByName("microphone");
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

	void MClientController::OnStartCommentaryClick(TinyVisual*, EventArgs& args)
	{
		TinyVisual* visual = m_view.GetDocument()->GetVisualByName("microphone");
		if (visual != NULL && visual->IsKindOf(RUNTIME_CLASS(TinyVisualComboBox)))
		{
			TinyVisualComboBox* val = static_cast<TinyVisualComboBox*>(visual);
			if (val->GetOptions().GetSize() == 0)
			{
				MessageBox(m_view.Handle(), "û�м�⵽��˷��豸", "��ʾ", MB_OK);
				return;
			}
		}
		if (Connect())
		{
			StartCommentary();
			MAudioDB* audiodb = static_cast<MAudioDB*>(m_view.GetDocument()->GetVisualByName("microphoneDB"));
			if (audiodb != NULL)
			{
				audiodb->SetDB(0);
				audiodb->SetVisible(TRUE);
				audiodb->Invalidate();
			}
		}
	}

	void MClientController::OnPauseCommentaryClick(TinyVisual*, EventArgs& args)
	{
		m_bPause = !m_bPause;
		TinyVisual* spvis = m_view.GetDocument()->GetVisualByName("btnPauseCommentary");
		if (spvis != NULL)
		{
			spvis->SetText(m_bPause ? "����" : "��ͣ");
		}
		m_audioDSP.Stop();
		if (!m_bPause)
		{
			m_audioDSP.Start();
		}
		MAudioDB* audiodb = static_cast<MAudioDB*>(m_view.GetDocument()->GetVisualByName("microphoneDB"));
		if (audiodb != NULL)
		{
			audiodb->SetDB(0);
			audiodb->SetVisible(TRUE);
		}
		m_view.Invalidate();
	}

	void MClientController::OnStopCommentaryClick(TinyVisual*, EventArgs& args)
	{
		if (MessageBox(m_view.Handle(), "ֹͣ��ֹͣ�������Ƿ���Ҫֹͣ", "����", MB_OKCANCEL) == IDOK)
		{
			if (Disconnect(m_szSourceID))
			{
				StopCommentary();
				MAudioDB* audiodb = static_cast<MAudioDB*>(m_view.GetDocument()->GetVisualByName("microphoneDB"));
				if (audiodb != NULL)
				{
					audiodb->SetDB(0);
					audiodb->SetVisible(FALSE);
					audiodb->Invalidate();
				}
			}
		}
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
			if (val->GetOptions().GetSize() == 0)
			{
				MessageBox(m_view.Handle(), "û�м�⵽��˷��豸!", "��ʾ", MB_OK);
			}
			else
			{
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
			if (val->GetOptions().GetSize() == 0)
			{
				MessageBox(m_view.Handle(), "û�м�⵽�����豸!", "��ʾ", MB_OK);
			}
			else
			{
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

	void MClientController::OnAudio(BYTE* bits, LONG size, INT db)
	{
		if (m_preview != NULL)
		{
			if (size == 4096)
			{
				LONGLONG currentPTS = MShow::MShowApp::GetInstance().GetCurrentAudioTS();
				if (m_previousPTS != currentPTS)
				{
					m_previousPTS = currentPTS;
				}
				else
				{
					LOG(INFO) << "OnAudio Timestamp repeat:" << currentPTS;
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
				if (m_view.GetDocument() != NULL && db > 0)
				{
					MAudioDB* audiodb = static_cast<MAudioDB*>(m_view.GetDocument()->GetVisualByName("microphoneDB"));
					if (audiodb != NULL && audiodb->IsVisible())
					{
						audiodb->SetDB(db);
						audiodb->Invalidate();
					}
				}
			}
		}
		else
		{
			LOG(ERROR) << "Preview NULL";
		}
	}

	TinyPerformanceTime g_timeQPC;

	void MClientController::OnMessagePump()
	{
		INT count = 0;
		AUDIO_SAMPLE sample;
		for (;;)
		{
			if (m_bBreak)
				break;
			if (m_audioSDK != NULL)
			{
				ZeroMemory(&sample, sizeof(sample));
				if (!m_audioQueue.Pop(sample, count))
				{
					Sleep(1);
					continue;
				}
				m_timeQPC.BeginTime();
				if (m_audioSDK->audio_encode_send(sample.bits + 4, static_cast<INT32>(sample.timestamp)) != 0)
				{
					LOG(INFO) << "Timestamp: " << sample.timestamp << " FAIL";
				}
				m_timeQPC.EndTime();
				LOG(INFO) << "audio_encode_send:" << m_timeQPC.GetMillisconds() << " Count:" << count << " Timestamp:" << sample.timestamp;
				TRACE("audio_encode_send:%lld, Count:%d, Timestamp:%lld\n", m_timeQPC.GetMillisconds(), count, sample.timestamp);
				m_audioQueue.Free(sample.bits);
			}
		}
	}
}