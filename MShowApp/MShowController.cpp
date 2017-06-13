#include "stdafx.h"
#include "MShowController.h"

namespace MShow
{
	MShowController::MShowController(DXView& view)
		:m_view(view)
	{
	}


	MShowController::~MShowController()
	{
	}

	BOOL MShowController::Initialize(LPCSTR pzURL)
	{
		RECT s = { 0 };
		::GetWindowRect(m_view.Handle(), &s);
		TinySize size = { TO_CX(s) ,TO_CY(s) };
		if (!m_d2d.Initialize(m_view.Handle(), size.cx, size.cy))
			return FALSE;
		if (!m_queue.Create())
			return FALSE;
		MFLVScene* pFLV = new MFLVScene(this);
		if (!pFLV->Initialize(m_d2d, pzURL))
		{
			SAFE_DELETE(pFLV);
			return FALSE;
		}
		if (!pFLV->Submit())
		{
			SAFE_DELETE(pFLV);
			return FALSE;
		}
		
		MGIFScene* pGIF = new MGIFScene(this);
		if (!pGIF->Initialize(m_d2d, "D:\\timg.gif"))
		{
			SAFE_DELETE(pGIF);
			return FALSE;
		}
		if (!pGIF->Submit())
		{
			SAFE_DELETE(pGIF);
			return FALSE;
		}

		m_scenes.Add(pFLV);
		m_scenes.Add(pGIF);
	
		m_onSize.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MShowController::OnSize));
		m_onLButtonDown.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MShowController::OnLButtonDown));
		m_onLButtonUp.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MShowController::OnLButtonUp));
		m_onRButtonDown.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MShowController::OnRButtonDown));
		m_onMouseMove.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MShowController::OnMouseMove));
		m_onMouseLeave.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MShowController::OnMouseLeave));
		m_onSetCursor.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MShowController::OnSetCursor));

		m_view.EVENT_SIZE += m_onSize;
		m_view.EVENT_LBUTTONDOWN += m_onLButtonDown;
		m_view.EVENT_LBUTTONUP += m_onLButtonUp;
		m_view.EVENT_RBUTTONDOWN += m_onRButtonDown;
		m_view.EVENT_MOUSEMOVE += m_onMouseMove;
		m_view.EVENT_MOUSELEAVE += m_onMouseLeave;
		m_view.EVENT_SETCURSOR += m_onSetCursor;
		return TRUE;
	}

	void MShowController::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		INT cx = LOWORD(lParam);
		INT cy = HIWORD(lParam);
	}

	void MShowController::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
	}

	void MShowController::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
	}

	void MShowController::OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;

	}

	void MShowController::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
	}

	void MShowController::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
	}

	void MShowController::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
	}

	void MShowController::Uninitialize()
	{
		m_queue.Destory();
		m_view.EVENT_SIZE -= m_onSize;
		m_view.EVENT_LBUTTONDOWN -= m_onLButtonDown;
		m_view.EVENT_LBUTTONUP -= m_onLButtonUp;
		m_view.EVENT_RBUTTONDOWN -= m_onRButtonDown;
		m_view.EVENT_MOUSEMOVE -= m_onMouseMove;
		m_view.EVENT_MOUSELEAVE -= m_onMouseLeave;
		m_view.EVENT_SETCURSOR -= m_onSetCursor;
		for (UINT i = 0;i < m_scenes.GetSize();i++)
		{
			m_scenes[i]->Close();
		}
	}

	TinyTimerQueue* MShowController::GetTimerQueue()
	{
		return &m_queue;
	}

	void MShowController::Draw(MElement* ps)
	{
		if (m_d2d.BeginDraw())
		{
			for (UINT i = 0;i < m_scenes.GetSize();i++)
			{
				m_scenes[i]->Draw(m_d2d);
			}
			m_d2d.EndDraw();
		}
	}
}
