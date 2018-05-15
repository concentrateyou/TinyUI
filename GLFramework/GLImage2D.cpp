#include "stdafx.h"
#include "GLImage2D.h"

namespace GLFramework
{
	IMPLEMENT_DYNAMIC(GLImage2D, GLImageElement2D);

	GLImage2D::GLImage2D()
	{

	}

	GLImage2D::~GLImage2D()
	{

	}

	BOOL GLImage2D::Initialize(GL& gl)
	{
		return TRUE;
	}

	BOOL GLImage2D::Create(GL& gl, INT cx, INT cy, const BYTE* bits)
	{
		return TRUE;
	}

	BOOL GLImage2D::Load(GL& gl, const BYTE* bits, LONG size)
	{
		return TRUE;
	}

	BOOL GLImage2D::Load(GL& gl, const CHAR* pzFile)
	{
		return TRUE;
	}

	void GLImage2D::Destory()
	{

	}
}