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

	void DX11Element2D::SetTrackingPosition(const TinyPoint& position)
	{
		m_trackerRect.SetPosition(position);
	}
	TinyPoint DX11Element2D::GetTrackingPosition() const
	{
		return m_trackerRect.Position();
	}
	void DX11Element2D::SetTrackingSize(const TinySize& scale)
	{
		m_trackerRect.SetSize(scale);
	}
	TinySize DX11Element2D::GetTrackingSize() const
	{
		return m_trackerRect.Size();
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
