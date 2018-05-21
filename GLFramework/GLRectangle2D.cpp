#include "stdafx.h"
#include "GLRectangle2D.h"

namespace GLFramework
{
	IMPLEMENT_DYNAMIC(GLRectangle2D, GLElement2D);

	GLRectangle2D::GLRectangle2D()
		:m_vertexArrayID(NULL),
		m_vertexID(NULL),
		m_indexID(NULL)
	{

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
		GLuint indices[6];
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 3;
		indices[3] = 1;
		indices[4] = 2;
		indices[5] = 3;
		GL::GetAPI().glGenVertexArrays(1, &m_vertexArrayID);
		GL::GetAPI().glBindVertexArray(m_vertexArrayID);
		GL::GetAPI().glGenBuffers(1, &m_vertexID);
		GL::GetAPI().glBindBuffer(GL_ARRAY_BUFFER, m_vertexID);
		GL::GetAPI().glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(VERTEXTYPE), vertexTypes, GL_STATIC_DRAW);
		GL::GetAPI().glEnableVertexAttribArray(0);
		GL::GetAPI().glEnableVertexAttribArray(1);
		GL::GetAPI().glEnableVertexAttribArray(2);
		GL::GetAPI().glBindBuffer(GL_ARRAY_BUFFER, m_vertexID);
		GL::GetAPI().glVertexAttribPointer(0, 3, GL_FLOAT, FALSE, sizeof(VERTEXTYPE), 0);
		GL::GetAPI().glBindBuffer(GL_ARRAY_BUFFER, m_vertexID);
		GL::GetAPI().glVertexAttribPointer(1, 2, GL_FLOAT, FALSE, sizeof(VERTEXTYPE), (BYTE*)NULL + (3 * sizeof(FLOAT)));
		GL::GetAPI().glBindBuffer(GL_ARRAY_BUFFER, m_vertexID);
		GL::GetAPI().glVertexAttribPointer(2, 4, GL_FLOAT, FALSE, sizeof(VERTEXTYPE), (BYTE*)NULL + (5 * sizeof(FLOAT)));
		GL::GetAPI().glGenBuffers(1, &m_indexID);
		GL::GetAPI().glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexID);
		GL::GetAPI().glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(UINT), indices, GL_STATIC_DRAW);
		GL_CHECK_ERROR(FALSE);
		return TRUE;
	}
	void GLRectangle2D::Destory()
	{
		GL::GetAPI().glDisableVertexAttribArray(0);
		GL::GetAPI().glDisableVertexAttribArray(1);
		GL::GetAPI().glBindBuffer(GL_ARRAY_BUFFER, 0);
		GL::GetAPI().glDeleteBuffers(1, &m_vertexID);
		GL::GetAPI().glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		GL::GetAPI().glDeleteBuffers(1, &m_indexID);
		GL::GetAPI().glBindVertexArray(0);
		GL::GetAPI().glDeleteVertexArrays(1, &m_vertexArrayID);
		GL_CHECK_ERROR();
	}
	BOOL GLRectangle2D::DrawRectangle(GL& gl, const XMFLOAT2 points[4], const XMFLOAT4& color)
	{
		return TRUE;
	}
	BOOL GLRectangle2D::FillRectangle(GL& gl, const XMFLOAT2 points[4], const XMFLOAT4& color)
	{
		TinySize size = gl.GetSize();
		XMFLOAT2 center(static_cast<FLOAT>(size.cx) / 2, static_cast<FLOAT>(size.cy) / 2);
		VERTEXTYPE vertices[4];
		for (INT i = 0; i < 4; i++)
		{
			vertices[i].position = XMFLOAT3(-center.x + points[i].x, center.y - points[i].y, 0.0F);
			vertices[i].color = color;
		}
		GL::GetAPI().glBindBuffer(GL_ARRAY_BUFFER, m_vertexID);
		GL::GetAPI().glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(VERTEXTYPE), vertices, GL_STATIC_DRAW);
		GL_CHECK_ERROR(FALSE);
		return TRUE;
	}
}