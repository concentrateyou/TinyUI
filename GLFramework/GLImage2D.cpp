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
		glGenVertexArrays(1, &m_vertexArrayID);
		glBindVertexArray(m_vertexArrayID);
		glGenBuffers(1, &m_vertexID);
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexID);
		glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(VERTEXTYPE), m_vertexTypes, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexID);
		glVertexAttribPointer(0, 3, GL_FLOAT, FALSE, sizeof(VERTEXTYPE), 0);
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexID);
		glVertexAttribPointer(1, 2, GL_FLOAT, FALSE, sizeof(VERTEXTYPE), (BYTE*)NULL + (3 * sizeof(FLOAT)));
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexID);
		glVertexAttribPointer(2, 4, GL_FLOAT, FALSE, sizeof(VERTEXTYPE), (BYTE*)NULL + (5 * sizeof(FLOAT)));
		return TRUE;
	}

	BOOL GLImage2D::Create(GL& gl, INT cx, INT cy, const BYTE* bits)
	{
		Destory();
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
		Destory();
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
		Destory();
		if (!Initialize(gl))
			return FALSE;
		if (!GLTexture2D::Load(gl, pzFile))
			return FALSE;
		m_size.x = static_cast<FLOAT>(m_image.GetSize().cx);
		m_size.y = static_cast<FLOAT>(m_image.GetSize().cy);
		return TRUE;
	}

	void GLImage2D::Destory()
	{
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &m_vertexID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &m_indexID);
		glBindVertexArray(0);
		glDeleteVertexArrays(1, &m_vertexArrayID);
		Destory();
	}
}