#include "stdafx.h"
#include "DX9Element2D.h"

namespace DXFramework
{
	IMPLEMENT_DYNAMIC(DX9Element2D, TinyObject);

	DX9Element2D::DX9Element2D()
		:m_bFlipH(FALSE),
		m_bFlipV(FALSE),
		m_angle(0.0F)
	{
		m_size.x = m_size.y = 0.0F;
		m_translate.x = m_translate.y = 0.0F;
		m_scale.x = m_scale.y = 1.0F;

	}
	DX9Element2D::~DX9Element2D()
	{
	}

	FLOAT	DX9Element2D::GetRotate() const
	{
		return m_angle;
	}
	XMFLOAT2	 DX9Element2D::GetSize() const
	{
		return m_size;
	}
	XMFLOAT2	 DX9Element2D::GetScale() const
	{
		return m_scale;
	}

	XMFLOAT2	 DX9Element2D::GetTranslate() const
	{
		return m_translate;
	}

	BOOL DX9Element2D::IsFlipH() const
	{
		return m_bFlipH;
	}

	BOOL DX9Element2D::IsFlipV() const
	{
		return m_bFlipV;
	}
	void DX9Element2D::SetSize(const XMFLOAT2& size)
	{
		m_size.x = size.x;
		m_size.y = size.y;
	}
	void DX9Element2D::SetRotate(FLOAT angle)
	{
		m_angle = angle;
	}

	void DX9Element2D::SetScale(const XMFLOAT2& scale)
	{
		m_scale.x = scale.x;
		m_scale.y = scale.y;
	}

	void DX9Element2D::SetTranslate(const XMFLOAT2& pos)
	{
		m_translate.x = pos.x;
		m_translate.y = pos.y;
	}

	void DX9Element2D::SetFlipH(BOOL bFlag)
	{
		m_bFlipH = bFlag;
	}

	void DX9Element2D::SetFlipV(BOOL bFlag)
	{
		m_bFlipV = bFlag;
	}

	void DX9Element2D::SetElementName(const TinyString& name)
	{
		m_szElement = std::move(name);
	}
	TinyString DX9Element2D::GetElementName() const
	{
		return m_szElement;
	}
}
