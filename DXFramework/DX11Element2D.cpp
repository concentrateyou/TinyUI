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
	void DX11Element2D::SetElementName(const TinyString& name)
	{
		m_szElement = std::move(name);
	}
	TinyString DX11Element2D::GetElementName() const
	{
		return m_szElement;
	}
}
