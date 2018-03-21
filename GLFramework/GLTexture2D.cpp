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
		glGenTextures(1, &m_textureID);
		glBindTexture(GL_TEXTURE_2D, m_textureID);
		//https://msdn.microsoft.com/en-us/library/windows/desktop/dd368638(v=vs.85).aspx
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cx, cy, 0, GL_BGRA, GL_UNSIGNED_BYTE, bits);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
		return TRUE;
	}
	BOOL GLTexture2D::Load(GL& dx11, const BYTE* bits, LONG size)
	{
		return TRUE;
	}
	BOOL GLTexture2D::Load(GL& gl, const CHAR* pzFile)
	{
		return TRUE;
	}
	void GLTexture2D::Destory()
	{

	}
}
