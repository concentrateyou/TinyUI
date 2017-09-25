#include "stdafx.h"
#include "MSearchController.h"
#include "Network/TinyHTTPClient.h"
using namespace TinyUI::Network;

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
		m_onItemClick.Reset(new Delegate<void(TinyVisual*, EventArgs&)>(this, &MSearchController::OnItemClick));
		return TRUE;
	}

	void MSearchController::OnMinimumClick(TinyVisual*, EventArgs& args)
	{
		m_view.GetDocument()->ReleaseCapture();//必须释放捕获
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

	void MSearchController::OnItemClick(TinyVisual*, EventArgs& args)
	{

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
		TinyString szName = search->GetText();
		string body;
		body += "pname=";
		body += szName.IsEmpty() ? "" : szName.CSTR();
		client.GetRequest().SetBody(body);
		if (!client.Open("http://10.23.84.150:7777/api/director/list"))
		{
			TRACE("[MSearchController][GetPrograms] Open Fail\n");
			LOG(ERROR) << "[MSearchController][GetPrograms] " << "Open Fail";
			lblMsg->SetVisible(TRUE);
			lblMsg->SetText("http://10.23.84.150:7777/api/director/list 打开失败");
			goto _ERROR;
		}
		if (!client.GetResponse().ReadAsString(context))
		{
			TRACE("[MSearchController][GetPrograms] Get Response Fail\n");
			LOG(ERROR) << "[MSearchController][GetPrograms] " << "Get Response Fail";
			lblMsg->SetVisible(TRUE);
			lblMsg->SetText("http://10.23.84.150:7777/api/director/list 读取数据失败");
			goto _ERROR;
		}
		if (!reader.parse(context, value))
		{
			TRACE("[MSearchController][GetPrograms] Parse Json Fail\n");
			LOG(ERROR) << "[MSearchController][GetPrograms] " << "Parse Json Fail";
			lblMsg->SetVisible(TRUE);
			lblMsg->SetText("http://10.23.84.150:7777/api/director/list Json解析失败");
			goto _ERROR;
		}
		list->RemoveAll();
		result = value["data"]["result"];
		TRACE("[MSearchController][GetPrograms] Program Count:%d\n", result.size());
		LOG(INFO) << "[MSearchController][GetPrograms] " << "Program Count: " << result.size();
		if (result.size() > 0)
		{
			list->SetVisible(TRUE);
			lblMsg->SetVisible(FALSE);
			for (INT i = 0;i < result.size();i++)
			{
				Json::Value val = result[i];
				string programName = val["programName"].asString();
				string imgUrl = val["imgUrl"].asString();
				list->Add(programName.c_str(), imgUrl.c_str());
			}
		}
		else
		{
			list->SetVisible(FALSE);
			lblMsg->SetVisible(TRUE);
			lblMsg->SetTextColor(RGB(128, 128, 128));
			lblMsg->SetText("啊哦，没有找到可以解说的节目。");
		}
	_ERROR:
		loading->EndAnimate();
		loading->SetVisible(FALSE);
		m_view.Invalidate();
	}
}
