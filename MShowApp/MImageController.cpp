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
		return TRUE;
	}

	BOOL MImageController::Open(LPCSTR pzFile)
	{
		TinySize size;
		TinyRectangle rectangle;
		m_image.Close();
		if (!m_image.Open(pzFile))
			goto _ERROR;
		size = m_image.GetSize();
		if (!m_image2D.Create(m_graphics.GetDX11(), size.cx, size.cy, FALSE))
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
		DWORD deDelay = m_image.GetCount() > 1 ? m_image.GetDelay(m_index) : 40;
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
		return m_image2D.GetHandle();
	}

	void MImageController::OnAdd()
	{
		LPCTSTR lpszFilter = _T("Image Files(*.bmp, *.jpg, *.png, *.gif)|*.bmp;*.jpg;*.png;*.gif;||");
		TinyFileDialog dlg(TRUE, NULL, "", OFN_HIDEREADONLY | OFN_READONLY | OFN_FILEMUSTEXIST, lpszFilter);
		if (dlg.DoModal(m_view.Handle()) == IDOK)
		{
			this->OnRemove();
			TinyString szName = dlg.GetPathName();
			if (!szName.IsEmpty())
			{
				this->Open(szName.STR());
			}
		}
	}

	void MImageController::OnRemove()
	{
		MPreviewController* preview = MShowApp::GetInstance().GetController().GetPreviewController();
		if (preview != NULL && m_pImage != NULL)
		{
			preview->Remove(m_pImage);
			m_pImage->Deallocate(preview->Graphics().GetDX11());
			SAFE_DELETE(m_pImage);
		}
		this->Close();
		m_view.Invalidate();
	}

	void MImageController::OnImage(BYTE* bits, LONG size)
	{
		TinySize imageSize = m_image.GetSize();
		if (m_image2D.Copy(m_graphics.GetDX11(), NULL, bits, size))
		{
			m_graphics.GetDX11().SetRenderTexture2D(NULL);
			m_graphics.GetDX11().GetRender2D()->BeginDraw();
			m_graphics.DrawImage(&m_image2D, 1.0F, 1.0F);
			m_graphics.GetDX11().GetRender2D()->EndDraw();
			m_graphics.Flush();
			m_graphics.Present();
		}
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
		MPreviewController* preview = MShowApp::GetInstance().GetController().GetPreviewController();
		if (preview != NULL)
		{
			if (m_pImage == NULL)
			{
				m_pImage = new MImageElement(*this);
				if (m_pImage->Allocate(preview->Graphics().GetDX11()))
				{
					preview->Add(m_pImage);
					preview->Bring(m_pImage, TRUE);
				}
			}
			else
			{
				preview->Add(m_pImage);
				preview->Bring(m_pImage, TRUE);
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
			INT count = pThis->m_image.GetCount();
			if (count > 1)
			{
				TinyApplication::GetInstance()->GetTimers().Change(pThis->m_handle, pThis->m_image.GetDelay(pThis->m_index), pThis->m_image.GetDelay(pThis->m_index));
				BYTE* bits = pThis->m_image.GetBits(pThis->m_index);
				TinySize size = pThis->m_image.GetSize();
				pThis->OnImage(bits, size.cx * size.cy * 4);
				pThis->m_index = (++pThis->m_index) % count;
			}
			else
			{
				BYTE* bits = pThis->m_image.GetBits(0);
				TinySize size = pThis->m_image.GetSize();
				pThis->OnImage(bits, size.cx * size.cy * 4);
			}
		}
	}

}

