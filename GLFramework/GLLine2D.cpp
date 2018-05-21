#include "stdafx.h"
#include "GLLine2D.h"

namespace GLFramework
{
	IMPLEMENT_DYNAMIC(GLLine2D, GLElement2D);

	GLLine2D::GLLine2D()
		:m_vertexArrayID(NULL),
		m_vertexID(NULL),
		m_indexID(NULL)
	{

	}
	GLLine2D::~GLLine2D()
	{
		Destory();
	}
	BOOL GLLine2D::Create(GL& gl)
	{
		return TRUE;
	}
	BOOL GLLine2D::DrawLine(GL& gl, const XMFLOAT2* points, DWORD count, const XMFLOAT4& color)
	{
		if (m_vertexTypes.GetSize() != count)
		{
			m_vertexTypes.Reset(count);
			if (!m_vertexTypes)
				return FALSE;
			ZeroMemory(m_vertexTypes.Ptr(), (sizeof(VERTEXTYPE) * count));
			GL::GetAPI().glGenVertexArrays(1, &m_vertexArrayID);
			GL::GetAPI().glBindVertexArray(m_vertexArrayID);
			GL::GetAPI().glGenBuffers(1, &m_vertexID);
			GL::GetAPI().glBindBuffer(GL_ARRAY_BUFFER, m_vertexID);
			GL::GetAPI().glBufferData(GL_ARRAY_BUFFER, count * sizeof(VERTEXTYPE), m_vertexTypes, GL_STATIC_DRAW);
			GL::GetAPI().glEnableVertexAttribArray(0);
			GL::GetAPI().glEnableVertexAttribArray(1);
			GL::GetAPI().glBindBuffer(GL_ARRAY_BUFFER, m_vertexID);
			GL::GetAPI().glVertexAttribPointer(0, 3, GL_FLOAT, FALSE, sizeof(VERTEXTYPE), 0);
			GL::GetAPI().glBindBuffer(GL_ARRAY_BUFFER, m_vertexID);
			GL::GetAPI().glVertexAttribPointer(1, 4, GL_FLOAT, FALSE, sizeof(VERTEXTYPE), (BYTE*)NULL + (3 * sizeof(FLOAT)));
			TinyScopedArray<UINT> indices(new UINT[count]);
			if (!indices)
				return FALSE;
			ZeroMemory(indices.Ptr(), (sizeof(UINT) * count));
			for (INT i = 0; i < count; i++)
			{
				indices[i] = i;
			}
			GL::GetAPI().glGenBuffers(1, &m_indexID);
			GL::GetAPI().glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexID);
			GL::GetAPI().glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(UINT), indices, GL_STATIC_DRAW);
			GL_CHECK_ERROR(FALSE);
		}
		else
		{
			if (!m_vertexTypes)
			{
				m_vertexTypes.Reset(count);
				if (!m_vertexTypes)
					return FALSE;
				GL::GetAPI().glGenVertexArrays(1, &m_vertexArrayID);
				GL::GetAPI().glBindVertexArray(m_vertexArrayID);
				GL::GetAPI().glGenBuffers(1, &m_vertexID);
				GL::GetAPI().glBindBuffer(GL_ARRAY_BUFFER, m_vertexID);
				GL::GetAPI().glBufferData(GL_ARRAY_BUFFER, count * sizeof(VERTEXTYPE), m_vertexTypes, GL_STATIC_DRAW);
				GL::GetAPI().glEnableVertexAttribArray(0);
				GL::GetAPI().glEnableVertexAttribArray(1);
				GL::GetAPI().glBindBuffer(GL_ARRAY_BUFFER, m_vertexID);
				GL::GetAPI().glVertexAttribPointer(0, 3, GL_FLOAT, FALSE, sizeof(VERTEXTYPE), 0);
				GL::GetAPI().glBindBuffer(GL_ARRAY_BUFFER, m_vertexID);
				GL::GetAPI().glVertexAttribPointer(1, 4, GL_FLOAT, FALSE, sizeof(VERTEXTYPE), (BYTE*)NULL + (3 * sizeof(FLOAT)));
				TinyScopedArray<UINT> indices(new UINT[count]);
				if (!indices)
					return FALSE;
				ZeroMemory(indices.Ptr(), (sizeof(UINT) * count));
				for (INT i = 0; i < count; i++)
				{
					indices[i] = i;
				}
				GL::GetAPI().glGenBuffers(1, &m_indexID);
				GL::GetAPI().glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexID);
				GL::GetAPI().glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(UINT), indices, GL_STATIC_DRAW);
				GL_CHECK_ERROR(FALSE);
			}
		}
		XMFLOAT2 size = gl.GetSize();
		XMFLOAT2 center(size.x / 2, size.y / 2);
		for (INT i = 0; i < count; i++)
		{
			m_vertexTypes[i].position = XMFLOAT3(-center.x + points[i].x, center.y - points[i].y, 0.0F);
			m_vertexTypes[i].color = color;
		}
		GL::GetAPI().glBindVertexArray(m_vertexArrayID);
		GL::GetAPI().glBindBuffer(GL_ARRAY_BUFFER, m_vertexID);
		GL::GetAPI().glBufferData(GL_ARRAY_BUFFER, count * sizeof(VERTEXTYPE), m_vertexTypes, GL_STATIC_DRAW);
		GL::GetAPI().glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexID);
		glDrawElements(GL_LINE_STRIP, count, GL_UNSIGNED_INT, 0);
		GL_CHECK_ERROR(FALSE);
		return TRUE;
	}
	void GLLine2D::Destory()
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
		if (m_indexID != NULL)
		{
			GL::GetAPI().glDeleteBuffers(1, &m_indexID);
			m_indexID = NULL;
		}
		GL::GetAPI().glBindVertexArray(0);
		if (m_vertexArrayID != NULL)
		{
			GL::GetAPI().glDeleteVertexArrays(1, &m_vertexArrayID);
			m_vertexArrayID = NULL;
		}
		GL_CHECK_ERROR();
	}
}