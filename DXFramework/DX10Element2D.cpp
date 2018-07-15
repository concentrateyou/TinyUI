#include "stdafx.h"
#include "DX10Element2D.h"

namespace DXFramework
{
	IMPLEMENT_DYNAMIC(DX10Element2D, TinyObject);

	DX10Element2D::DX10Element2D()
	{
	}
	DX10Element2D::~DX10Element2D()
	{
	}
	void DX10Element2D::SetElementName(const TinyString& name)
	{
		m_szElement = std::move(name);
	}
	TinyString DX10Element2D::GetElementName() const
	{
		return m_szElement;
	}
}
