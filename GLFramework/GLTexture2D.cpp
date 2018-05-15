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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, cx, cy, GL_RGBA, GL_UNSIGNED_BYTE, bits);
		return TRUE;
	}
	BOOL GLTexture2D::Load(GL& dx11, const BYTE* bits, LONG size)
	{
		if (!m_image.Open(const_cast<BYTE*>(bits), size))
			return FALSE;
		glGenTextures(1, &m_textureID);
		glBindTexture(GL_TEXTURE_2D, m_textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, m_image.GetSize().cx, m_image.GetSize().cy, GL_RGBA, GL_UNSIGNED_BYTE, m_image.GetBits());
		return TRUE;
	}
	BOOL GLTexture2D::Load(GL& gl, const CHAR* pzFile)
	{
		if (!m_image.Open(pzFile))
			return FALSE;
		glGenTextures(1, &m_textureID);
		glBindTexture(GL_TEXTURE_2D, m_textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, m_image.GetSize().cx, m_image.GetSize().cy, GL_RGBA, GL_UNSIGNED_BYTE, m_image.GetBits());
		return TRUE;
	}
	void GLTexture2D::Destory(GL& gl)
	{
		if (m_textureID != 0)
		{
			glDeleteTextures(1, &m_textureID);
			m_textureID = 0;
		}
	}
	GLTexture2D::operator GLuint() const
	{
		return m_textureID;
	}
	GLuint	GLTexture2D::GetTexture2D() const
	{
		return m_textureID;
	}
}
