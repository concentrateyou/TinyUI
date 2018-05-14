#include "stdafx.h"
#include "GLElement2D.h"

namespace GLFramework
{
	IMPLEMENT_DYNAMIC(GLElement2D, TinyObject);

	GLElement2D::GLElement2D()
	{
	}

	GLElement2D::~GLElement2D()
	{
	}
	void GLElement2D::SetElementName(const TinyString& name)
	{
		m_szElement = std::move(name);
	}
	TinyString GLElement2D::GetElementName() const
	{
		return m_szElement;
	}
}
