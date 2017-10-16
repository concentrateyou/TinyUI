#include "stdafx.h"
#include "MSearchController.h"
#include "Network/TinyHTTPClient.h"
#include "MShow.h"
using namespace TinyUI::Network;
using namespace TinyUI::Windowless;

namespace MShow
{
	MSearchController::MSearchController(MSearchWindow& view)
		:m_view(view)
	{

	}

	MSearchController::~MSearchController()
	{

	}

	BOOL MSearchController::Initialize()
	{
		TinyVisual* visual = m_view.GetDocument()->GetVisualByName("sysmin");
		if (visual != NULL)
		{
			m_onMinimumClick.Reset(new Delegate<void(TinyVisual*, EventArgs&)>(this, &MSearchController::OnMinimumClick));
			visual->EVENT_CLICK += m_onMinimumClick;
		}
		visual = m_view.GetDocument()->GetVisualByName("sysclose");
		if (visual != NULL)
		{
			m_onCloseClick.Reset(new Delegate<void(TinyVisual*, EventArgs&)>(this, &MSearchController::OnCloseClick));
			visual->EVENT_CLICK += m_onCloseClick;
		}
		visual = m_view.GetDocument()->GetVisualByName("btnSearch");
		if (visual != NULL)
		{
			m_onSearchClick.Reset(new Delegate<void(TinyVisual*, EventArgs&)>(this, &MSearchController::OnSearchClick));
			visual->EVENT_CLICK += m_onSearchClick;
		}
		m_onItemClick.Reset(new Delegate<void(TinyVisual*, MouseEventArgs&)>(this, &MSearchController::OnItemClick));
		return TRUE;
	}

	BOOL MSearchController::Uninitialize()
	{
		return FALSE;
	}

	void MSearchController::OnMinimumClick(TinyVisual*, EventArgs& args)
	{
		m_view.GetDocument()->ReleaseCapture();//�����ͷŲ���
		SendMessage(m_view.Handle(), WM_SYSCOMMAND, SC_MINIMIZE, NULL);
	}

	void MSearchController::OnCloseClick(TinyVisual*, EventArgs& args)
	{
		SendMessage(m_view.Handle(), WM_CLOSE, NULL, NULL);
	}

	void MSearchController::OnSearchClick(TinyVisual*, EventArgs& args)
	{
		TinyVisual* lblMsg = static_cast<TinyVisualAnimation*>(m_view.GetDocument()->GetVisualByName("lblMsg"));
		ASSERT(lblMsg);
		lblMsg->SetVisible(FALSE);
		TinyVisualAnimation* loading = static_cast<TinyVisualAnimation*>(m_view.GetDocument()->GetVisualByName("loading"));
		ASSERT(loading);
		loading->SetVisible(TRUE);
		loading->BeginAnimate();
		TinyVisualList* list = static_cast<TinyVisualList*>(m_view.GetDocument()->GetVisualByName("programs"));
		ASSERT(list);
		list->SetVisible(FALSE);
		m_view.Invalidate();
		m_task.Submit(BindCallback(&MSearchController::OnMessagePump, this));
	}

	void MSearchController::OnItemClick(TinyVisual* spvis, MouseEventArgs& args)
	{
		if (spvis->IsKindOf(RUNTIME_CLASS(TinyVisualListItem)))
		{
			TinyVisualListItem* ps = static_cast<TinyVisualListItem*>(spvis);
			SEARCH_ITEM* val = static_cast<SEARCH_ITEM*>(ps->GetItemData());
			if (val != NULL)
			{
				m_view.ShowWindow(SW_HIDE);
				m_view.UpdateWindow();
				MShow::MShowApp::GetInstance().GetClientView().ShowWindow(SW_NORMAL);
				MShow::MShowApp::GetInstance().GetClientController().SetProgram(val->szProgramName, val->szProgramID, val->szLogID);
				MShow::MShowApp::GetInstance().GetClientController().SetTimes(val->szBeginTime, val->szEndTime);
				MShow::MShowApp::GetInstance().GetClientController().SetPreview(val->szPreviewURL);
				MShow::MShowApp::GetInstance().GetClientView().UpdateWindow();
			}
		}
	}

	void MSearchController::OnMessagePump()
	{
		GetPrograms();
	}

	void MSearchController::GetPrograms()
	{
		TinyVisual* lblMsg = static_cast<TinyVisualAnimation*>(m_view.GetDocument()->GetVisualByName("lblMsg"));
		ASSERT(lblMsg);
		TinyVisualList* list = static_cast<TinyVisualList*>(m_view.GetDocument()->GetVisualByName("programs"));
		ASSERT(list);
		TinyVisualTextBox* search = static_cast<TinyVisualTextBox*>(m_view.GetDocument()->GetVisualByName("txtSearch"));
		ASSERT(search);
		TinyVisualAnimation* loading = static_cast<TinyVisualAnimation*>(m_view.GetDocument()->GetVisualByName("loading"));
		ASSERT(loading);
		string context;
		Json::Reader reader;
		Json::Value value;
		Json::Value result;
		TinyHTTPClient client;
		client.GetRequest().SetVerbs(TinyHTTPClient::POST);
		client.GetRequest().Add(TinyHTTPClient::ContentType, "application/x-www-form-urlencoded");
		client.GetRequest().Add("Sign", "#f93Uc31K24()_@");
		TinyString szName = search->GetText();
		string body;
		body += "pname=";
		body += szName.IsEmpty() ? "" : szName.CSTR();
		body += "&fullMode=true";
		client.GetRequest().SetBody(body);
		string address = StringPrintf("%s/%s", MShow::MShowApp::GetInstance().AppConfig().GetPrefix().c_str(), "list");
		if (!client.Open(address))
		{
			TRACE("[MSearchController][GetPrograms] Open Fail\n");
			LOG(ERROR) << "[MSearchController][GetPrograms] " << "Open Fail";
			lblMsg->SetVisible(TRUE);
			lblMsg->SetTextColor(RGB(255, 0, 0));
			lblMsg->SetText("http://10.23.84.150:7777/api/director/list ��ʧ��");
			goto _ERROR;
		}
		if (!client.GetResponse().ReadAsString(context))
		{
			TRACE("[MSearchController][GetPrograms] Get Response Fail\n");
			LOG(ERROR) << "[MSearchController][GetPrograms] " << "Get Response Fail";
			lblMsg->SetVisible(TRUE);
			lblMsg->SetTextColor(RGB(255, 0, 0));
			lblMsg->SetText("http://10.23.84.150:7777/api/director/list ��ȡ����ʧ��");
			goto _ERROR;
		}
		if (!reader.parse(context, value))
		{
			TRACE("[MSearchController][GetPrograms] Parse Json Fail\n");
			LOG(ERROR) << "[MSearchController][GetPrograms] " << "Parse Json Fail";
			lblMsg->SetVisible(TRUE);
			lblMsg->SetTextColor(RGB(255, 0, 0));
			lblMsg->SetText("http://10.23.84.150:7777/api/director/list Json����ʧ��");
			goto _ERROR;
		}

		list->RemoveAll(TRUE);
		result = value["data"]["result"];
		TRACE("[MSearchController][GetPrograms] Program Count:%d\n", result.size());
		LOG(INFO) << "[MSearchController][GetPrograms] " << "Program Count: " << result.size();
		if (result.size() > 0)
		{
			list->SetVisible(TRUE);
			list->GetVScrollBar()->SetVisible(FALSE);
			lblMsg->SetVisible(FALSE);
			for (INT i = 0;i < result.size();i++)
			{
				Json::Value val = result[i];
				string programName = val["programName"].asString();
				string imgUrl = val["imgUrl"].asString();
				TinyVisualListItem* listitem = list->Add(programName.c_str(), imgUrl.c_str());
				if (listitem != NULL)
				{
					SEARCH_ITEM* ps = new SEARCH_ITEM();
					ps->szLogID = std::move(val["id"].asString());
					ps->szBeginTime = std::move(val["startPlayTime"].asString());
					ps->szEndTime = std::move(val["stopPlayTime"].asString());
					ps->szPreviewURL = std::move(val["previewStreamUrl"].asString());
					ps->szProgramName = std::move(val["programName"].asString());
					ps->szProgramID = std::move(val["programQipuId"].asString());
					listitem->SetItemData(ps);
					listitem->EVENT_MOUSEDBCLICK += m_onItemClick;
				}
			}
		}
		else
		{
			list->SetVisible(FALSE);
			lblMsg->SetVisible(TRUE);
			lblMsg->SetTextColor(RGB(128, 128, 128));
			lblMsg->SetText("��Ŷ��û���ҵ����Խ�˵�Ľ�Ŀ��");
		}
	_ERROR:
		loading->EndAnimate();
		loading->SetVisible(FALSE);
		m_view.Invalidate();
	};
}