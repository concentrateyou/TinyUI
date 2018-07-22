#include "stdafx.h"
#include "DX10Element.h"

namespace DXFramework
{
	IMPLEMENT_DYNAMIC(DX10Element, TinyObject);

	DX10Element::DX10Element()
	{
	}
	DX10Element::~DX10Element()
	{
	}
	void DX10Element::SetElementName(const TinyString& name)
	{
		m_szElement = std::move(name);
	}
	TinyString DX10Element::GetElementName() const
	{
		return m_szElement;
	}
}
