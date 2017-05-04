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

	void DX11Element2D::SetPosition(const TinyPoint& position)
	{
		m_trackerRect.SetPosition(position);
	}
	TinyPoint DX11Element2D::GetPosition() const
	{
		return m_trackerRect.Position();
	}
	void DX11Element2D::SetScale(const TinySize& scale)
	{
		m_trackerRect.SetSize(scale);
	}
	void DX11Element2D::SetSize(const TinySize& size)
	{
		m_size = size;
	}
	TinySize DX11Element2D::GetScale() const
	{
		return m_trackerRect.Size();
	}
	TinySize DX11Element2D::GetSize() const
	{
		return m_size;
	}
	BOOL DX11Element2D::PtInRect(const TinyPoint& pos)
	{
		return m_trackerRect.PtInRect(pos);
	}
	void DX11Element2D::SetName(const TinyString& name)
	{
		m_name = std::move(name);
	}
	TinyString DX11Element2D::GetName() const
	{
		return m_name;
	}
}
