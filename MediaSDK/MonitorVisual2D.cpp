#include "stdafx.h"
#include "MonitorVisual2D.h"

namespace MediaSDK
{
	IMPLEMENT_DYNAMIC(MonitorVisual2D, IVisual2D);

	MonitorVisual2D::MonitorVisual2D(DX11& dx11)
		:m_dx11(dx11),
		m_index(0),
		m_bCursor(FALSE)
	{

	}

	MonitorVisual2D::~MonitorVisual2D()
	{
	}

	XMFLOAT2 MonitorVisual2D::GetTranslate()
	{
		return m_visual2D.GetTranslate();
	}
	XMFLOAT2 MonitorVisual2D::GetScale()
	{
		return m_visual2D.GetScale();
	}
	XMFLOAT2 MonitorVisual2D::GetSize()
	{
		return m_visual2D.GetSize();
	}

	void MonitorVisual2D::SetTranslate(const XMFLOAT2& pos)
	{
		m_visual2D.SetTranslate(pos);
	}

	void MonitorVisual2D::SetScale(const XMFLOAT2& pos)
	{
		m_visual2D.SetScale(pos);
	}
	DX11Image2D& MonitorVisual2D::GetCursor()
	{
		return m_cursor2D;
	}
	UINT MonitorVisual2D::GetOutputCount() const
	{
		return m_duplicator.GetOutputCount(m_dx11);
	}
	BOOL MonitorVisual2D::SetOutput(UINT32 index, BOOL bCursor)
	{
		m_index = index;
		m_bCursor = bCursor;
		return TRUE;
	}
	BOOL MonitorVisual2D::UpdateCursor()
	{
		HICON hICON = NULL;
		ICONINFO ii = { 0 };
		CURSORINFO ci = { 0 };
		ci.cbSize = sizeof(ci);
		if (!GetCursorInfo(&ci))
			goto _ERROR;
		hICON = CopyIcon(ci.hCursor);
		if (!GetIconInfo(hICON, &ii))
			goto _ERROR;
		if (ci.flags == CURSOR_SHOWING)
		{
			BITMAP bmp = { 0 };
			if (GetObject(hICON, sizeof(BITMAP), &bmp) != 0)
			{
				if (m_cursor2D.IsEmpty())
				{
					if (!m_cursor2D.Create(m_dx11, bmp.bmWidth, bmp.bmHeight, NULL, FALSE))
						goto _ERROR;
				}
				UINT size = bmp.bmWidthBytes * bmp.bmHeight;
				TinyScopedPtr<BYTE>	color(new BYTE[size]);
				ASSERT(color);
				GetBitmapBits(ii.hbmColor, size, color);
				D3D11_MAPPED_SUBRESOURCE map;
				if (m_cursor2D.Map(m_dx11, map))
				{
					UINT32 linesize = min(map.RowPitch, bmp.bmWidthBytes);
					memcpy((BYTE*)map.pData, color, linesize * bmp.bmHeight);
					m_cursor2D.Unmap(m_dx11);
				}
				m_cursor2D.SetScale(m_visual2D.GetScale());
				m_cursor2D.SetTranslate(XMFLOAT2(static_cast<FLOAT>(ci.ptScreenPos.x), static_cast<FLOAT>(ci.ptScreenPos.y)));
			}
		}
		SAFE_DELETE_OBJECT(ii.hbmColor);
		SAFE_DELETE_OBJECT(ii.hbmMask);
		if (hICON != NULL)
		{
			DestroyIcon(hICON);
			hICON = NULL;
		}
		return TRUE;
	_ERROR:
		SAFE_DELETE_OBJECT(ii.hbmColor);
		SAFE_DELETE_OBJECT(ii.hbmMask);
		if (hICON != NULL)
		{
			DestroyIcon(hICON);
			hICON = NULL;
		}
		return FALSE;
	}
	BOOL MonitorVisual2D::Open()
	{
		m_duplicator.Close();
		if (!m_duplicator.Open(m_dx11, m_index))
			return FALSE;
		HANDLE handle = m_duplicator.GetSharedHandle();
		if (!m_visual2D.Load(m_dx11, handle))
			return FALSE;
		/*	if (!UpdateCursor())
				return FALSE;*/
		return TRUE;
	}
	BOOL MonitorVisual2D::Tick()
	{
		if (!m_duplicator.AcquireNextFrame(m_dx11, 0))
			return FALSE;
		/*if (!UpdateCursor())
			return FALSE;*/
		return TRUE;
	}
	void MonitorVisual2D::Close()
	{
		m_duplicator.Close();
		m_visual2D.Destory();
	}
	LPCSTR	MonitorVisual2D::GetVisualName()
	{
		return TEXT("MonitorVisual2D");
	}
	DX11Image2D* MonitorVisual2D::GetVisual2D()
	{
		return &m_visual2D;
	}
}