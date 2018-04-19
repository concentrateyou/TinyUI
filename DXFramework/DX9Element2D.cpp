#include "stdafx.h"
#include "DX9Element2D.h"

namespace DXFramework
{
	IMPLEMENT_DYNAMIC(DX9Element2D, TinyObject);

	DX9Element2D::DX9Element2D()
	{
	}
	DX9Element2D::~DX9Element2D()
	{
	}

	TinyRectangle DX9Element2D::GetViewport() const
	{
		return m_trackerRect;
	}

	void DX9Element2D::SetViewport(const TinyRectangle& s)
	{
		m_trackerRect.SetRect(s.Position(), s.Size());
	}
	void DX9Element2D::SetViewport(const TinyPoint& pos, const TinySize& size)
	{
		m_trackerRect.SetRect(pos, size);
	}
	BOOL DX9Element2D::PtInRect(const TinyPoint& pos)
	{
		return m_trackerRect.PtInRect(pos);
	}
	void DX9Element2D::SetElementName(const TinyString& name)
	{
		m_szElement = std::move(name);
	}
	TinyString DX9Element2D::GetElementName() const
	{
		return m_szElement;
	}
}
