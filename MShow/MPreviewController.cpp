#include "stdafx.h"
#include "MPreviewController.h"
#include "WICTexture.h"

namespace MShow
{
#define IDM_MOVEUP		100
#define IDM_MOVEDOWN	101
#define IDM_MOVETOP		102
#define IDM_MOVEBOTTPM	103
#define IDM_REMOVE		104

	MPreviewController::MPreviewController(MPreviewView& view)
		:m_view(view),
		m_player(BindCallback(&MPreviewController::OnVideo, this))
	{

	}

	MPreviewController::~MPreviewController()
	{

	}

	BOOL MPreviewController::Initialize()
	{
		TinyRectangle s;
		GetClientRect(m_view.Handle(), &s);
		m_viewSize = s.Size();
		if (!m_dx2d.Initialize(m_view.Handle(), TO_CX(s), TO_CY(s)))
			return FALSE;
		return TRUE;
	}

	BOOL MPreviewController::Open(LPCSTR pzURL)
	{
		D2D1_BITMAP_PROPERTIES1 properties = { { DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED }, 96, 96, D2D1_BITMAP_OPTIONS_TARGET, 0 };
		if (!m_player.Open(m_view.Handle(), pzURL))
			goto _ERROR;
		m_videoSize = m_player.GetSize();
		D2D1_SIZE_U bitmapPixelSize = D2D1::SizeU((UINT)(m_videoSize.cx), (UINT)(m_videoSize.cy));
		HRESULT hRes = m_dx2d.GetContext()->CreateBitmap(bitmapPixelSize, NULL, 0, properties, &m_bitmap1);
		if (FAILED(hRes))
			goto _ERROR;
		return TRUE;
	_ERROR:
		m_player.Close();
		return FALSE;
	}

	BOOL MPreviewController::Close()
	{
		if (m_player.Close())
		{
			m_bitmap1.Release();
			return TRUE;
		}
		return FALSE;
	}

	MPreviewView& MPreviewController::GetView()
	{
		return m_view;
	}

	void MPreviewController::OnVideo(BYTE* bits, LONG size)
	{
		if (m_bitmap1 != NULL)
		{
			ASSERT(size == m_videoSize.cx * m_videoSize.cy * 4);
			m_bitmap1->CopyFromMemory(NULL, bits, m_videoSize.cx * 4);
			m_dx2d.BeginDraw();
			D2D1_RECT_F dest = { 0.F,0.F,m_viewSize.cx,m_viewSize.cy };
			m_dx2d.GetContext()->DrawBitmap(m_bitmap1, &dest, 1.0F, D2D1_INTERPOLATION_MODE_LINEAR, NULL, NULL);
			m_dx2d.EndDraw();
		}
	}
}
