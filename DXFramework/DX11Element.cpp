#include "stdafx.h"
#include "DX11Element.h"

namespace DXFramework
{
	DX11Element::DX11Element()
	{
	}

	DX11Element::~DX11Element()
	{
	}
	void DX11Element::SetPosition(const TinyPoint& position)
	{
		m_position = position;
	}
	TinyPoint DX11Element::GetPosition() const
	{
		return m_position;
	}
	void DX11Element::SetScale(const TinySize& scale)
	{
		m_scale = scale;
	}
	TinySize DX11Element::GetScale() const
	{
		return m_scale;
	}
}
