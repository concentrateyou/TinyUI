#include "stdafx.h"
#include "DX11Element2D.h"

namespace DXFramework
{
	IMPLEMENT_DYNAMIC(DX11Element2D, TinyObject);

	DX11Element2D::DX11Element2D()
	{
	}
	DX11Element2D::~DX11Element2D()
	{
	}

	TinyRectangle DX11Element2D::GetRectangle() const
	{
		return m_trackerRect;
	}
	void DX11Element2D::SetRectangle(const TinyRectangle& s)
	{
		m_trackerRect.SetRect(s.Position(), s.Size());
	}
	void DX11Element2D::SetRectangle(const TinyPoint& pos, const TinySize& size)
	{
		m_trackerRect.SetRect(pos, size);
	}
	BOOL DX11Element2D::PtInRect(const TinyPoint& pos)
	{
		return m_trackerRect.PtInRect(pos);
	}
	void DX11Element2D::SetElementName(const TinyString& name)
	{
		m_szElement = std::move(name);
	}
	TinyString DX11Element2D::GetElementName() const
	{
		return m_szElement;
	}
}
