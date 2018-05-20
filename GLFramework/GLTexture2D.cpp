#include "stdafx.h"
#include "GLTexture2D.h"


namespace GLFramework
{
	GLTexture2D::GLTexture2D()
		:m_textureID(NULL)
	{

	}

	GLTexture2D::~GLTexture2D()
	{
	}

	BOOL GLTexture2D::Create(GL& gl, INT cx, INT cy, const BYTE* bits)
	{
		GL::GetAPI().glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &m_textureID);
		glBindTexture(GL_TEXTURE_2D, m_textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cx, cy, 0, GL_RGBA, GL_UNSIGNED_BYTE, bits);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		GL::GetAPI().glGenerateMipmap(GL_TEXTURE_2D);
		return TRUE;
	}
	BOOL GLTexture2D::Load(GL& dx11, const BYTE* bits, LONG size)
	{
		if (!m_image.Open(const_cast<BYTE*>(bits), size))
			return FALSE;
		GL::GetAPI().glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &m_textureID);
		glBindTexture(GL_TEXTURE_2D, m_textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_image.GetSize().cx, m_image.GetSize().cy, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_image.GetBits());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		GL::GetAPI().glGenerateMipmap(GL_TEXTURE_2D);
		return TRUE;
	}
	BOOL GLTexture2D::Load(GL& gl, const CHAR* pzFile)
	{
		if (!m_image.Open(pzFile))
			return FALSE;
		GL::GetAPI().glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &m_textureID);
		glBindTexture(GL_TEXTURE_2D, m_textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_image.GetSize().cx, m_image.GetSize().cy, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_image.GetBits());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
		GL::GetAPI().glGenerateMipmap(GL_TEXTURE_2D);
		return TRUE;
	}
	void GLTexture2D::Destory()
	{
		if (m_textureID != NULL)
		{
			if (glIsTexture(m_textureID))
			{
				glDeleteTextures(1, &m_textureID);
			}
			m_textureID = NULL;
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
	BOOL GLTexture2D::IsEmpty() const
	{
		return m_textureID == NULL;
	}
}
