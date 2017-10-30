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

	void DX9Element2D::SetPosition(const TinyPoint& position)
	{
		m_trackerRect.SetPosition(position);
	}
	TinyPoint DX9Element2D::GetPosition() const
	{
		return m_trackerRect.Position();
	}
	void DX9Element2D::SetScale(const TinySize& scale)
	{
		m_trackerRect.SetSize(scale);
	}
	void DX9Element2D::SetSize(const TinySize& size)
	{
		m_size = size;
	}
	TinySize DX9Element2D::GetScale() const
	{
		return m_trackerRect.Size();
	}
	TinySize DX9Element2D::GetSize() const
	{
		return m_size;
	}
	BOOL DX9Element2D::PtInRect(const TinyPoint& pos)
	{
		return m_trackerRect.PtInRect(pos);
	}
	void DX9Element2D::SetName(const TinyString& name)
	{
		m_name = std::move(name);
	}
	TinyString DX9Element2D::GetName() const
	{
		return m_name;
	}
}
