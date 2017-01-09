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
		m_rectangle.SetPosition(position);
	}
	TinyPoint DX11Element::GetPosition() const
	{
		return *(TinyPoint*)&m_rectangle;
	}
	void DX11Element::SetScale(const TinySize& scale)
	{
		m_rectangle.SetSize(scale);
	}
	void DX11Element::SetSize(const TinySize& size)
	{
		m_size = size;
	}
	TinySize DX11Element::GetScale() const
	{
		return *((TinySize*)&m_rectangle + 1);
	}
	TinySize DX11Element::GetSize() const
	{
		return m_size;
	}
}
