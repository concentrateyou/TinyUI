#include "stdafx.h"
#include "MImageController.h"
#include "MPreviewController.h"
#include "MImageElement.h"
#include "MShowApp.h"
#include "Control/TinyCommonDialog.h"
#include "resource.h"

namespace MShow
{
	MImageController::MImageController(MImageView& view)
		:m_view(view),
		m_pImage(NULL),
		m_index(0),
		m_handle(NULL)
	{
		m_onLButtonDBClick.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MImageController::OnLButtonDBClick));
		m_onRButtonDown.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MImageController::OnRButtonDown));
		m_view.EVENT_LDBCLICK += m_onLButtonDBClick;
		m_view.EVENT_RBUTTONDOWN += m_onRButtonDown;
		m_onMenuClick.Reset(new Delegate<void(void*, INT)>(this, &MImageController::OnMenuClick));
		m_popup.EVENT_CLICK += m_onMenuClick;
	}

	MImageController::~MImageController()
	{
		m_view.EVENT_LDBCLICK -= m_onLButtonDBClick;
		m_view.EVENT_RBUTTONDOWN -= m_onRButtonDown;
		m_popup.EVENT_CLICK -= m_onMenuClick;
		m_popup.DestroyMenu();
	}

	BOOL MImageController::Initialize()
	{
		TinyRectangle rectangle;
		m_view.GetClientRect(&rectangle);
		if (!m_graphics.Initialize(m_view.Handle(), rectangle.Size()))
			return FALSE;
		m_popup.CreatePopupMenu();
		m_popup.AppendMenu(MF_STRING, 1, TEXT("添加"));
		m_popup.AppendMenu(MF_STRING, 2, TEXT("删除"));
		m_signal.CreateEvent();
		return TRUE;
	}

	BOOL MImageController::Open(LPCSTR pzFile)
	{
		TinyRectangle rectangle;
		if (!m_image2D.Load(m_graphics.GetDX11(), pzFile))
			goto _ERROR;
		m_view.GetClientRect(&rectangle);
		m_image2D.SetScale(rectangle.Size());
		m_szFile = pzFile;
		TinyTimerQueue& queue = TinyApplication::GetInstance()->GetTimers();
		if (m_handle != NULL)
		{
			queue.Unregister(m_handle);
			m_handle = NULL;
		}
		DWORD deDelay = 40;
		m_handle = queue.Register(&MImageController::TimerCallback, this, deDelay, deDelay, WT_EXECUTEINTIMERTHREAD);
		return m_handle != NULL;
	_ERROR:
		m_image2D.Destory();
		return FALSE;
	}

	BOOL MImageController::Close()
	{
		TinyTimerQueue& queue = TinyApplication::GetInstance()->GetTimers();
		if (m_handle != NULL)
		{
			queue.Unregister(m_handle);
			m_handle = NULL;
		}
		m_image2D.Destory();
		return TRUE;
	}

	TinyString	MImageController::GetFile() const
	{
		return m_szFile;
	}

	HANDLE MImageController::GetHandle()
	{
		DX11Texture2D* ps = m_image2D.GetTexture2D();
		if (ps && !ps->IsEmpty())
		{
			return ps->GetHandle();
		}
		return NULL;
	}

	void MImageController::OnAdd()
	{
		LPCTSTR lpszFilter = _T("Image Files(*.bmp, *.jpg, *.png, *.gif)|*.bmp;*.jpg;*.png;*.gif|All Files (*.*)|*.*||");
		TinyFileDialog dlg(TRUE, NULL, "", OFN_HIDEREADONLY | OFN_READONLY | OFN_FILEMUSTEXIST, lpszFilter);
		if (dlg.DoModal(m_view.Handle()) == IDOK)
		{
			this->OnRemove();
			TinyString szName = dlg.GetPathName();
			if (!szName.IsEmpty())
			{
				if (this->Open(szName.STR()))
				{
					MPreviewController* preview = MShowApp::Instance().GetController().GetPreviewController();
					if (preview != NULL)
					{
						preview->m_waits.push_back(m_signal);
					}
				}
			}
		}
	}

	void MImageController::OnRemove()
	{
		MPreviewController* preview = MShowApp::Instance().GetController().GetPreviewController();
		if (preview != NULL && m_pImage != NULL)
		{
			preview->Remove(m_pImage);
			m_pImage->Deallocate(preview->Graphics().GetDX11());
			SAFE_DELETE(m_pImage);
			m_signal.SetEvent();
			auto s = std::find(preview->m_waits.begin(), preview->m_waits.end(), m_signal);
			if (s != preview->m_waits.end())
			{
				preview->m_waits.erase(s);
			}
		}
		this->Close();
		m_view.Invalidate();
	}

	void MImageController::OnImage()
	{
		m_graphics.GetDX11().SetRenderTexture2D(NULL);
		m_graphics.GetDX11().GetRender2D()->BeginDraw();
		FLOAT blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		m_graphics.GetDX11().AllowBlend(TRUE, blendFactor);
		m_graphics.GetDX11().AllowDepth(FALSE);
		m_graphics.DrawImage(&m_image2D, 1.0F, 1.0F);
		m_graphics.GetDX11().AllowDepth(TRUE);
		m_graphics.GetDX11().AllowBlend(FALSE, blendFactor);
		m_graphics.GetDX11().GetRender2D()->EndDraw();
		m_graphics.Present();
		m_signal.SetEvent();
	}

	void MImageController::OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		TinyPoint point(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		m_view.ClientToScreen(&point);
		m_popup.TrackPopupMenu(TPM_LEFTBUTTON, point.x, point.y, m_view.Handle(), NULL);
	}

	void MImageController::OnLButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		MPreviewController* preview = MShowApp::Instance().GetController().GetPreviewController();
		if (preview != NULL)
		{
			if (m_pImage == NULL)
			{
				m_pImage = new MImageElement(*this);
				if (m_pImage->Allocate(preview->Graphics().GetDX11()))
				{
					preview->Add(m_pImage);
				}
			}
			else
			{
				preview->Add(m_pImage);
			}
		}
	}

	void MImageController::OnMenuClick(void*, INT wID)
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

	VOID CALLBACK MImageController::TimerCallback(PVOID lpParameter, BOOLEAN val)
	{
		MImageController* pThis = reinterpret_cast<MImageController*>(lpParameter);
		if (pThis && val)
		{
			pThis->OnImage();
		}
	}

}

