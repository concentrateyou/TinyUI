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
		if (!m_cursor2D.Create(m_dx11, ci.hCursor))
		{
			DestroyIcon(hICON);
			hICON = NULL;
		}
		return TRUE;
	}
	BOOL MonitorVisual2D::Tick()
	{
		if (!m_duplicator.AcquireNextFrame(m_dx11, 0))
			return FALSE;
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