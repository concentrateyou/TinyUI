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
			m_onMinimumClick.Reset(new Delegate<void(EventArgs&)>(this, &MSearchController::OnMinimumClick));
			visual->EVENT_CLICK += m_onMinimumClick;
		}
		visual = m_view.GetDocument()->GetVisualByName("sysclose");
		if (visual != NULL)
		{
			m_onCloseClick.Reset(new Delegate<void(EventArgs&)>(this, &MSearchController::OnCloseClick));
			visual->EVENT_CLICK += m_onCloseClick;
		}
		visual = m_view.GetDocument()->GetVisualByName("btnSearch");
		if (visual != NULL)
		{
			m_onSearchClick.Reset(new Delegate<void(EventArgs&)>(this, &MSearchController::OnSearchClick));
			visual->EVENT_CLICK += m_onSearchClick;
		}
		return TRUE;
	}

	void MSearchController::OnMinimumClick(EventArgs& args)
	{
		m_view.GetDocument()->ReleaseCapture();//±ÿ–Î Õ∑≈≤∂ªÒ
		SendMessage(m_view.Handle(), WM_SYSCOMMAND, SC_MINIMIZE, NULL);
	}

	void MSearchController::OnCloseClick(EventArgs& args)
	{
		SendMessage(m_view.Handle(), WM_CLOSE, NULL, NULL);
	}

	void MSearchController::OnSearchClick(EventArgs& args)
	{
		GetPrograms();
	}

	void MSearchController::GetPrograms()
	{
		TinyVisualTextBox* txtSearch = static_cast<TinyVisualTextBox*>(m_view.GetDocument()->GetVisualByName("txtSearch"));
		if (txtSearch != NULL)
		{
			TinyHTTPClient client;
			client.GetRequest().SetVerbs(TinyHTTPClient::POST);
			client.GetRequest().Add(TinyHTTPClient::ContentType, "application/x-www-form-urlencoded");
			TinyString szName = txtSearch->GetText();
			string body;
			body += "pname=";
			body += szName.IsEmpty() ? "" : szName.CSTR();
			client.GetRequest().SetBody(body);
			if (client.Open("http://10.23.84.150:7777/api/director/list"))
			{
				string context = client.GetResponse().GetContext();
				Json::Reader reader;
				Json::Value value;
				if (reader.parse(context, value))
				{
					Json::Value result = value["data"]["result"];
					for (INT i = 0;i < result.size();i++)
					{
						Json::Value val = result[i];
						string str1 = val["programQipuId"].asString();
						string str2 = val["programName"].asString();
						string str3 = val["startPlayTime"].asString();
						string str4 = val["stopPlayTime"].asString();
						string str5 = val["programQipuId"].asString();
					}
				}
			}
		}
	}
}
