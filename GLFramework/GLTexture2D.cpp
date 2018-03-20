#include "stdafx.h"
#include "GLTexture2D.h"

namespace GLFramework
{
	GLTexture2D::GLTexture2D()
		:m_textureID(0)
	{
	}

	GLTexture2D::~GLTexture2D()
	{
	}

	BOOL GLTexture2D::Create(GL& gl, INT cx, INT cy, const BYTE* bits)
	{
		return TRUE;
	}
}
