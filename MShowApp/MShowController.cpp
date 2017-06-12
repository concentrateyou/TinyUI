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
		m_onVideo.Reset(new Delegate<void(ID2D1Bitmap1*)>(this, &MShowController::OnVideo));
		m_task.EVENT_VIDEO += m_onVideo;
	}

	void MShowController::OnVideo(ID2D1Bitmap1* bitmap)
	{
		if (m_d2d.BeginDraw())
		{
			FLV_SCRIPTDATA& script = m_task.GetScript();
			D2D_SIZE_F sf = m_d2d.GetContext()->GetSize();
			D2D_RECT_F dst = { 0.0F,0.0F,sf.width,sf.height };
			D2D_RECT_F src = { 0.0F,0.0F,script.width,script.height };
			m_d2d.GetContext()->DrawBitmap(bitmap, dst, 1.0F, D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC, src, NULL);
			m_d2d.EndDraw();
		}
	}

	void MShowController::Uninitialize()
	{
		m_task.EVENT_VIDEO -= m_onVideo;
		m_task.Close(INFINITE);
	}
}
