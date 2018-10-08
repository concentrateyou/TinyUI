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
	void MonitorVisual2D::SetName(LPCSTR pzName)
	{
		m_szNAME = pzName;
	}
	void MonitorVisual2D::SetTranslate(const XMFLOAT2& pos)
	{
		m_visual2D.SetTranslate(pos);
	}

	void MonitorVisual2D::SetScale(const XMFLOAT2& pos)
	{
		m_visual2D.SetScale(pos);
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

	BOOL MonitorVisual2D::Open()
	{
		m_duplicator.Close();
		if (!m_duplicator.Open(m_dx11, m_index))
			return FALSE;
		HANDLE handle = m_duplicator.GetSharedHandle();
		if (!m_visual2D.Load(m_dx11, handle))
			return FALSE;
		CURSORINFO ci;
		ZeroMemory(&ci, sizeof(ci));
		ci.cbSize = sizeof(ci);
		if (!GetCursorInfo(&ci))
			return FALSE;
		HICON hICON = CopyIcon(ci.hCursor);
		BOOL bRes = m_cursor2D.Create(m_dx11, hICON);
		DestroyIcon(hICON);
		hICON = NULL;
		return bRes;
	}
	BOOL MonitorVisual2D::Tick(INT64& timestamp)
	{
		if (!m_duplicator.AcquireNextFrame(m_dx11, 0))
			return FALSE;
		CURSORINFO ci;
		ZeroMemory(&ci, sizeof(ci));
		ci.cbSize = sizeof(ci);
		if (!GetCursorInfo(&ci))
			return FALSE;
		TinySize size(static_cast<LONG>(m_cursor2D.GetSize().x), static_cast<LONG>(m_cursor2D.GetSize().y));
		HICON hICON = CopyIcon(ci.hCursor);
		ICONINFO ii;
		ZeroMemory(&ii, sizeof(ii));
		if (!GetIconInfo(hICON, &ii))
			return FALSE;
		BOOL bRes = m_cursor2D.UpdateCursor(m_dx11, hICON, size);
		DestroyIcon(hICON);
		hICON = NULL;
		m_cursor2D.SetFlipH(m_visual2D.IsFlipH());
		m_cursor2D.SetFlipV(m_visual2D.IsFlipV());
		m_cursor2D.SetRotate(m_visual2D.GetRotate());
		m_cursor2D.SetScale(m_visual2D.GetScale());
		POINT pos = ci.ptScreenPos;
		pos.x -= static_cast<LONG>(ii.xHotspot);
		pos.y -= static_cast<LONG>(ii.yHotspot);
		XMFLOAT2 scale = m_visual2D.GetScale();
		XMFLOAT2 translate = m_visual2D.GetTranslate();
		m_cursor2D.SetTranslate(XMFLOAT2(translate.x + static_cast<FLOAT>(pos.x) * scale.x, translate.y + static_cast<FLOAT>(pos.y) * scale.y));
		return bRes;
	}
	BOOL MonitorVisual2D::Draw(DX11Graphics2D& g)
	{
		FLOAT blendFactor[4] = { 0.0F, 0.0F, 0.0F, 0.0F };
		m_dx11.AllowBlend(TRUE, blendFactor);
		if (g.DrawImage(m_visual2D))
		{
			CURSORINFO ci;
			ZeroMemory(&ci, sizeof(ci));
			ci.cbSize = sizeof(ci);
			if (!GetCursorInfo(&ci))
				return FALSE;
			if (ci.flags & CURSOR_SHOWING)
			{
				return g.DrawImage(m_cursor2D);
			}
		}
		return FALSE;
	}
	void MonitorVisual2D::Close()
	{
		m_duplicator.Close();
		m_visual2D.Destory();
		m_cursor2D.Destory();
	}
	LPCSTR	MonitorVisual2D::GetName()
	{
		return m_szNAME.CSTR();
	}
}