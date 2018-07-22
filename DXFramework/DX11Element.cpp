#include "stdafx.h"
#include "DX11Element.h"

namespace DXFramework
{
	IMPLEMENT_DYNAMIC(DX11Element, TinyObject);

	DX11Element::DX11Element()
	{
	}
	DX11Element::~DX11Element()
	{
	}
	void DX11Element::SetElementName(const TinyString& name)
	{
		m_szElement = std::move(name);
	}
	TinyString DX11Element::GetElementName() const
	{
		return m_szElement;
	}
}
