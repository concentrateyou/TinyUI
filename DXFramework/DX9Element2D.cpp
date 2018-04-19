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

	void DX9Element2D::SetTrackingPosition(const TinyPoint& position)
	{
		m_trackerRect.OffsetRect(position);
	}
	TinyPoint DX9Element2D::GetTrackingPosition() const
	{
		return m_trackerRect.Position();
	}
	void DX9Element2D::SetTrackingSize(const TinySize& scale)
	{
		m_trackerRect.SetSize(scale);
	}

	TinySize DX9Element2D::GetTrackingSize() const
	{
		return m_trackerRect.Size();
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
