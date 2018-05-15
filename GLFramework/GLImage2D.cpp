#include "stdafx.h"
#include "GLImage2D.h"

namespace GLFramework
{
	IMPLEMENT_DYNAMIC(GLImage2D, GLImageElement2D);

	GLImage2D::GLImage2D()
		:m_vertexArrayID(0),
		m_vertexID(0),
		m_indexID(0)
	{
		ZeroMemory(m_vertexTypes, (sizeof(VERTEXTYPE) * 6));
		ZeroMemory(m_indices, (sizeof(UINT) * 6));
	}

	GLImage2D::~GLImage2D()
	{

	}

	BOOL GLImage2D::Initialize(GL& gl)
	{
		gl.glGenVertexArrays(1, &m_vertexArrayID);
		gl.glBindVertexArray(m_vertexArrayID);
		gl.glGenBuffers(1, &m_vertexID);
		gl.glBindBuffer(GL_ARRAY_BUFFER, m_vertexID);
		gl.glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(VERTEXTYPE), m_vertexTypes, GL_STATIC_DRAW);
		gl.glEnableVertexAttribArray(0);
		gl.glEnableVertexAttribArray(1);
		gl.glEnableVertexAttribArray(2);
		gl.glBindBuffer(GL_ARRAY_BUFFER, m_vertexID);
		gl.glVertexAttribPointer(0, 3, GL_FLOAT, FALSE, sizeof(VERTEXTYPE), 0);
		gl.glBindBuffer(GL_ARRAY_BUFFER, m_vertexID);
		gl.glVertexAttribPointer(1, 2, GL_FLOAT, FALSE, sizeof(VERTEXTYPE), (BYTE*)NULL + (3 * sizeof(FLOAT)));
		gl.glBindBuffer(GL_ARRAY_BUFFER, m_vertexID);
		gl.glVertexAttribPointer(2, 4, GL_FLOAT, FALSE, sizeof(VERTEXTYPE), (BYTE*)NULL + (5 * sizeof(FLOAT)));
		return TRUE;
	}

	BOOL GLImage2D::Create(GL& gl, INT cx, INT cy, const BYTE* bits)
	{
		Destory(gl);
		if (!Initialize(gl))
			return FALSE;
		if (!GLTexture2D::Create(gl, cx, cy, bits))
			return FALSE;
		m_size.x = static_cast<FLOAT>(m_image.GetSize().cx);
		m_size.y = static_cast<FLOAT>(m_image.GetSize().cy);
		return TRUE;
	}

	BOOL GLImage2D::Load(GL& gl, const BYTE* bits, LONG size)
	{
		Destory(gl);
		if (!Initialize(gl))
			return FALSE;
		if (!GLTexture2D::Load(gl, bits, size))
			return FALSE;
		m_size.x = static_cast<FLOAT>(m_image.GetSize().cx);
		m_size.y = static_cast<FLOAT>(m_image.GetSize().cy);
		return TRUE;
	}

	BOOL GLImage2D::Load(GL& gl, const CHAR* pzFile)
	{
		Destory(gl);
		if (!Initialize(gl))
			return FALSE;
		if (!GLTexture2D::Load(gl, pzFile))
			return FALSE;
		m_size.x = static_cast<FLOAT>(m_image.GetSize().cx);
		m_size.y = static_cast<FLOAT>(m_image.GetSize().cy);
		return TRUE;
	}

	void GLImage2D::Destory(GL& gl)
	{
		gl.glDisableVertexAttribArray(0);
		gl.glDisableVertexAttribArray(1);
		gl.glBindBuffer(GL_ARRAY_BUFFER, 0);
		gl.glDeleteBuffers(1, &m_vertexID);
		gl.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		gl.glDeleteBuffers(1, &m_indexID);
		gl.glBindVertexArray(0);
		gl.glDeleteVertexArrays(1, &m_vertexArrayID);
		Destory(gl);
	}
}