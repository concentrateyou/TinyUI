#include "stdafx.h"
#include "GLImageElement2D.h"

namespace GLFramework
{
	IMPLEMENT_DYNAMIC(GLImageElement2D, GLElement2D);

	GLImageElement2D::GLImageElement2D()
		:m_bFlipH(FALSE),
		m_bFlipV(FALSE),
		m_angle(0.0F)
	{
		m_size.x = m_size.y = 0.0F;
		m_translate.x = m_translate.y = 0.0F;
		m_scale.x = m_scale.y = 1.0F;
	}
	GLImageElement2D::~GLImageElement2D()
	{
	}
	FLOAT GLImageElement2D::GetRotate() const
	{
		return m_angle;
	}
	XMFLOAT2 GLImageElement2D::GetSize() const
	{
		return m_size;
	}
	XMFLOAT2 GLImageElement2D::GetScale() const
	{
		return m_scale;
	}
	XMFLOAT2 GLImageElement2D::GetTranslate() const
	{
		return m_translate;
	}
	BOOL GLImageElement2D::IsFlipH() const
	{
		return m_bFlipH;
	}
	BOOL GLImageElement2D::IsFlipV() const
	{
		return m_bFlipV;
	}
	void GLImageElement2D::SetSize(const XMFLOAT2& size)
	{
		m_size = size;
	}
	void GLImageElement2D::SetRotate(FLOAT angle)
	{
		m_angle = angle;
	}
	void GLImageElement2D::SetScale(const XMFLOAT2& scale)
	{
		m_scale = scale;
	}
	void GLImageElement2D::SetTranslate(const XMFLOAT2& pos)
	{
		m_translate = pos;
	}
	void GLImageElement2D::SetFlipH(BOOL bFlag)
	{
		m_bFlipH = bFlag;
	}
	void GLImageElement2D::SetFlipV(BOOL bFlag)
	{
		m_bFlipV = bFlag;
	}
}
