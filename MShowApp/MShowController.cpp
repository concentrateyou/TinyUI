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
		if (!m_task.Initialize(m_d2d, pzURL))
			return FALSE;
		if (!m_task.Submit())
			return FALSE;
		m_onVideo.Reset(new Delegate<void(ID2D1Bitmap1*, INT)>(this, &MShowController::OnVideo));
		m_onSize.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MShowController::OnSize));
		m_onLButtonDown.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MShowController::OnLButtonDown));
		m_onLButtonUp.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MShowController::OnLButtonUp));
		m_onRButtonDown.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MShowController::OnRButtonDown));
		m_onMouseMove.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MShowController::OnMouseMove));
		m_onMouseLeave.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MShowController::OnMouseLeave));
		m_onSetCursor.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MShowController::OnSetCursor));
		m_task.EVENT_VIDEO += m_onVideo;
		m_view.EVENT_SIZE += m_onSize;
		m_view.EVENT_LBUTTONDOWN += m_onLButtonDown;
		m_view.EVENT_LBUTTONUP += m_onLButtonUp;
		m_view.EVENT_RBUTTONDOWN += m_onRButtonDown;
		m_view.EVENT_MOUSEMOVE += m_onMouseMove;
		m_view.EVENT_MOUSELEAVE += m_onMouseLeave;
		m_view.EVENT_SETCURSOR += m_onSetCursor;
		m_gifScene.Initialize(m_d2d, "D:\\timg.gif");
		return TRUE;
	}

	void MShowController::OnVideo(ID2D1Bitmap1* bitmap, INT delay)
	{
		if (m_d2d.BeginDraw())
		{
			//m_timer.BeginTime();
			FLV_SCRIPTDATA& script = m_task.GetScript();
			D2D_SIZE_F sf = m_d2d.GetContext()->GetSize();
			D2D_RECT_F dst = { 0.0F,0.0F,sf.width,sf.height };
			D2D_RECT_F src = { 0.0F,0.0F,script.width,script.height };
			m_d2d.GetContext()->DrawBitmap(bitmap, dst, 1.0F, D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC, src, NULL);
			//m_timer.EndTime();
			//delay -= m_timer.GetMillisconds();
			//m_gifScene.Draw(m_d2d, delay);
			m_d2d.EndDraw();
		}
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
		m_task.EVENT_VIDEO -= m_onVideo;
		m_view.EVENT_SIZE -= m_onSize;
		m_view.EVENT_LBUTTONDOWN -= m_onLButtonDown;
		m_view.EVENT_LBUTTONUP -= m_onLButtonUp;
		m_view.EVENT_RBUTTONDOWN -= m_onRButtonDown;
		m_view.EVENT_MOUSEMOVE -= m_onMouseMove;
		m_view.EVENT_MOUSELEAVE -= m_onMouseLeave;
		m_view.EVENT_SETCURSOR -= m_onSetCursor;
		m_task.Close(INFINITE);
	}
}
