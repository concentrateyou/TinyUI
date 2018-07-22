#include "stdafx.h"
#include "DX11Texture2DElement.h"

namespace DXFramework
{
	IMPLEMENT_DYNAMIC(DX11Texture2DElement, DX11Element);

	DX11Texture2DElement::DX11Texture2DElement()
		:m_bFlipH(FALSE),
		m_bFlipV(FALSE),
		m_angle(0.0F)
	{
		m_size.x = m_size.y = 0.0F;
		m_translate.x = m_translate.y = 0.0F;
		m_scale.x = m_scale.y = 1.0F;
	}
	DX11Texture2DElement::~DX11Texture2DElement()
	{
	}
	FLOAT DX11Texture2DElement::GetRotate() const
	{
		return m_angle;
	}
	XMFLOAT2 DX11Texture2DElement::GetSize() const
	{
		return m_size;
	}
	XMFLOAT2 DX11Texture2DElement::GetScale() const
	{
		return m_scale;
	}
	XMFLOAT2 DX11Texture2DElement::GetTranslate() const
	{
		return m_translate;
	}
	BOOL DX11Texture2DElement::IsFlipH() const
	{
		return m_bFlipH;
	}
	BOOL DX11Texture2DElement::IsFlipV() const
	{
		return m_bFlipV;
	}
	void DX11Texture2DElement::SetSize(const XMFLOAT2& size)
	{
		m_size = size;
	}
	void DX11Texture2DElement::SetRotate(FLOAT angle)
	{
		m_angle = angle;
	}
	void DX11Texture2DElement::SetScale(const XMFLOAT2& scale)
	{
		m_scale = scale;
	}
	void DX11Texture2DElement::SetTranslate(const XMFLOAT2& pos)
	{
		m_translate = pos;
	}
	void DX11Texture2DElement::SetFlipH(BOOL bFlag)
	{
		m_bFlipH = bFlag;
	}
	void DX11Texture2DElement::SetFlipV(BOOL bFlag)
	{
		m_bFlipV = bFlag;
	}
}

