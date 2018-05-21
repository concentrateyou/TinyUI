#include "stdafx.h"
#include "GLRectangle2D.h"

namespace GLFramework
{
	IMPLEMENT_DYNAMIC(GLRectangle2D, GLElement2D);

	GLRectangle2D::GLRectangle2D()
		:m_vertexArrayID(NULL),
		m_vertexID(NULL)
	{
		m_indexs[0] = m_indexs[1] = NULL;
	}
	GLRectangle2D::~GLRectangle2D()
	{
		Destory();
	}
	BOOL GLRectangle2D::Create(GL& gl)
	{
		Destory();
		VERTEXTYPE vertexTypes[4];
		ZeroMemory(vertexTypes, (sizeof(VERTEXTYPE) * 4));
		GL::GetAPI().glGenVertexArrays(1, &m_vertexArrayID);
		GL::GetAPI().glBindVertexArray(m_vertexArrayID);
		GL::GetAPI().glGenBuffers(1, &m_vertexID);
		GL::GetAPI().glBindBuffer(GL_ARRAY_BUFFER, m_vertexID);
		GL::GetAPI().glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(VERTEXTYPE), vertexTypes, GL_STATIC_DRAW);
		GL::GetAPI().glEnableVertexAttribArray(0);
		GL::GetAPI().glEnableVertexAttribArray(1);
		GL::GetAPI().glBindBuffer(GL_ARRAY_BUFFER, m_vertexID);
		GL::GetAPI().glVertexAttribPointer(0, 3, GL_FLOAT, FALSE, sizeof(VERTEXTYPE), 0);
		GL::GetAPI().glBindBuffer(GL_ARRAY_BUFFER, m_vertexID);
		GL::GetAPI().glVertexAttribPointer(1, 4, GL_FLOAT, FALSE, sizeof(VERTEXTYPE), (BYTE*)NULL + (3 * sizeof(FLOAT)));
		GL::GetAPI().glGenBuffers(1, &m_indexs[0]);
		GL::GetAPI().glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexs[0]);
		GLuint indices1[5];
		indices1[0] = 0;
		indices1[1] = 1;
		indices1[2] = 2;
		indices1[3] = 3;
		indices1[4] = 0;
		GL::GetAPI().glBufferData(GL_ELEMENT_ARRAY_BUFFER, 5 * sizeof(UINT), indices1, GL_STATIC_DRAW);
		GL::GetAPI().glGenBuffers(1, &m_indexs[1]);
		GL::GetAPI().glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexs[1]);
		GLuint indices2[6];
		indices2[0] = 0;
		indices2[1] = 1;
		indices2[2] = 2;
		indices2[3] = 2;
		indices2[4] = 3;
		indices2[5] = 0;
		GL::GetAPI().glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(UINT), indices2, GL_STATIC_DRAW);
		GL_CHECK_ERROR(FALSE);
		return TRUE;
	}
	void GLRectangle2D::Destory()
	{
		GL::GetAPI().glDisableVertexAttribArray(0);
		GL::GetAPI().glDisableVertexAttribArray(1);
		GL::GetAPI().glBindBuffer(GL_ARRAY_BUFFER, 0);
		if (m_vertexID != NULL)
		{
			GL::GetAPI().glDeleteBuffers(1, &m_vertexID);
			m_vertexID = NULL;
		}
		GL::GetAPI().glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		if (m_indexs[0] != NULL)
		{
			GL::GetAPI().glDeleteBuffers(1, &m_indexs[0]);
			m_indexs[0] = NULL;
		}
		if (m_indexs[1] != NULL)
		{
			GL::GetAPI().glDeleteBuffers(1, &m_indexs[1]);
			m_indexs[1] = NULL;
		}
		GL::GetAPI().glBindVertexArray(0);
		if (m_vertexArrayID != NULL)
		{
			GL::GetAPI().glDeleteVertexArrays(1, &m_vertexArrayID);
			m_vertexArrayID = NULL;
		}
		GL_CHECK_ERROR();
	}
	BOOL GLRectangle2D::DrawRectangle(GL& gl, const XMFLOAT2 points[4], const XMFLOAT4& color)
	{
		XMFLOAT2 size = gl.GetSize();
		XMFLOAT2 center(size.x / 2, size.y / 2);
		VERTEXTYPE vertices[4];
		for (INT i = 0; i < 4; i++)
		{
			vertices[i].position = XMFLOAT3(-center.x + points[i].x, center.y - points[i].y, 0.0F);
			vertices[i].color = color;
		}
		GL::GetAPI().glBindVertexArray(m_vertexArrayID);
		GL::GetAPI().glBindBuffer(GL_ARRAY_BUFFER, m_vertexID);
		GL::GetAPI().glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(VERTEXTYPE), vertices, GL_STATIC_DRAW);
		GL::GetAPI().glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexs[0]);
		glDrawElements(GL_LINE_STRIP, 5, GL_UNSIGNED_INT, 0);
		GL_CHECK_ERROR(FALSE);
		return TRUE;
		return TRUE;
	}
	BOOL GLRectangle2D::FillRectangle(GL& gl, const XMFLOAT2 points[4], const XMFLOAT4& color)
	{
		XMFLOAT2 size = gl.GetSize();
		XMFLOAT2 center(size.x / 2, size.y / 2);
		VERTEXTYPE vertices[4];
		for (INT i = 0; i < 4; i++)
		{
			vertices[i].position = XMFLOAT3(-center.x + points[i].x, center.y - points[i].y, 0.0F);
			vertices[i].color = color;
		}
		GL::GetAPI().glBindVertexArray(m_vertexArrayID);
		GL::GetAPI().glBindBuffer(GL_ARRAY_BUFFER, m_vertexID);
		GL::GetAPI().glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(VERTEXTYPE), vertices, GL_STATIC_DRAW);
		GL::GetAPI().glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexs[1]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		GL_CHECK_ERROR(FALSE);
		return TRUE;
	}
}