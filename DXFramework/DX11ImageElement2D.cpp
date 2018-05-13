#include "stdafx.h"
#include "DX11ImageElement2D.h"

namespace DXFramework
{
	IMPLEMENT_DYNAMIC(DX11ImageElement2D, DX11Element2D);

	DX11ImageElement2D::DX11ImageElement2D()
		:m_bFlipH(FALSE),
		m_bFlipV(FALSE),
		m_angle(0.0F)
	{
		m_size.x = m_size.y = 0.0F;
		m_translate.x = m_translate.y = 0.0F;
		m_scale.x = m_scale.y = 1.0F;
	}
	DX11ImageElement2D::~DX11ImageElement2D()
	{
	}
	FLOAT DX11ImageElement2D::GetRotate() const
	{
		return m_angle;
	}
	XMFLOAT2 DX11ImageElement2D::GetSize() const
	{
		return m_size;
	}
	XMFLOAT2 DX11ImageElement2D::GetScale() const
	{
		return m_scale;
	}
	XMFLOAT2 DX11ImageElement2D::GetTranslate() const
	{
		return m_translate;
	}
	BOOL DX11ImageElement2D::IsFlipH() const
	{
		return m_bFlipH;
	}
	BOOL DX11ImageElement2D::IsFlipV() const
	{
		return m_bFlipV;
	}
	void DX11ImageElement2D::SetSize(const XMFLOAT2& size)
	{
		m_size = size;
	}
	void DX11ImageElement2D::SetRotate(FLOAT angle)
	{
		m_angle = angle;
	}
	void DX11ImageElement2D::SetScale(const XMFLOAT2& scale)
	{
		m_scale = scale;
	}
	void DX11ImageElement2D::SetTranslate(const XMFLOAT2& pos)
	{
		m_translate = pos;
	}
	void DX11ImageElement2D::SetFlipH(BOOL bFlag)
	{
		m_bFlipH = bFlag;
	}
	void DX11ImageElement2D::SetFlipV(BOOL bFlag)
	{
		m_bFlipV = bFlag;
	}
}
