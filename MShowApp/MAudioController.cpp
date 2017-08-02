#include "stdafx.h"
#include "MAudioController.h"
#include "MPreviewController.h"
#include "MImageElement.h"
#include "MShowApp.h"
#include "Control/TinyCommonDialog.h"
#include "resource.h"

namespace MShow
{
	MAudioController::MAudioController(MAudioView& view)
		:m_view(view)
	{
		m_onLButtonDBClick.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MAudioController::OnLButtonDBClick));
		m_onRButtonDown.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MAudioController::OnRButtonDown));
		m_view.EVENT_LDBCLICK += m_onLButtonDBClick;
		m_view.EVENT_RBUTTONDOWN += m_onRButtonDown;
		m_onMenuClick.Reset(new Delegate<void(void*, INT)>(this, &MAudioController::OnMenuClick));
		m_popup.EVENT_CLICK += m_onMenuClick;
	}

	MAudioController::~MAudioController()
	{
		m_view.EVENT_LDBCLICK -= m_onLButtonDBClick;
		m_view.EVENT_RBUTTONDOWN -= m_onRButtonDown;
		m_popup.EVENT_CLICK -= m_onMenuClick;
		m_popup.DestroyMenu();
	}

	BOOL MAudioController::Initialize()
	{
		m_popup.CreatePopupMenu();
		m_popup.AppendMenu(MF_STRING, 1, TEXT("添加"));
		m_popup.AppendMenu(MF_STRING, 2, TEXT("删除"));
		m_popup.AppendMenu(MF_STRING, 3, TEXT("播放"));
		return TRUE;
	}

	BOOL MAudioController::Open(LPCSTR pzFile)
	{
		m_szFile = pzFile;
		return FALSE;
	}

	BOOL MAudioController::Close()
	{
		return FALSE;
	}

	TinyString	MAudioController::GetFile() const
	{
		return m_szFile;
	}

	void MAudioController::OnAdd()
	{
		LPCTSTR lpszFilter = _T("MP3 Files(*.mp3)|*.mp3;||");
		TinyFileDialog dlg(TRUE, NULL, "", OFN_HIDEREADONLY | OFN_READONLY | OFN_FILEMUSTEXIST, lpszFilter);
		if (dlg.DoModal(m_view.Handle()) == IDOK)
		{
			m_szFile = dlg.GetPathName();
		}
	}

	void MAudioController::OnRemove()
	{
		this->Close();
		m_view.Invalidate();
	}


	void MAudioController::OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		TinyPoint point(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		m_view.ClientToScreen(&point);
		m_popup.TrackPopupMenu(TPM_LEFTBUTTON, point.x, point.y, m_view.Handle(), NULL);
	}

	void MAudioController::OnLButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;

	}

	void MAudioController::OnMenuClick(void*, INT wID)
	{
		switch (wID)
		{
		case 1:
			OnAdd();
			break;
		case 2:
			OnRemove();
			break;
		}
	}

}

