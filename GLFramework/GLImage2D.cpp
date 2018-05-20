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
		ZeroMemory(m_vertexTypes, (sizeof(VERTEXTYPE) * 4));
		ZeroMemory(m_indices, (sizeof(UINT) * 6));
	}

	GLImage2D::~GLImage2D()
	{

	}
	BOOL GLImage2D::Calculate(GL& gl)
	{
		if (!m_vertexID)
			return FALSE;
		m_vertexTypes[0].position = XMFLOAT3(-m_size.x / 2, -m_size.y / 2, 0.0F);
		m_vertexTypes[0].texture = XMFLOAT2(m_bFlipH ? 1.0F : 0.0F, m_bFlipV ? 0.0F : 1.0F);
		m_vertexTypes[0].color = XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F);
		m_vertexTypes[1].position = XMFLOAT3(-m_size.x / 2, m_size.y / 2, 0.0F);
		m_vertexTypes[1].texture = XMFLOAT2(m_bFlipH ? 1.0F : 0.0F, m_bFlipV ? 1.0F : 0.0F);
		m_vertexTypes[1].color = XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F);
		m_vertexTypes[2].position = XMFLOAT3(m_size.x / 2, m_size.y / 2, 0.0F);
		m_vertexTypes[2].texture = XMFLOAT2(m_bFlipH ? 0.0F : 1.0F, m_bFlipV ? 0.0F : 0.0F);
		m_vertexTypes[2].color = XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F);
		m_vertexTypes[3].position = XMFLOAT3(m_size.x / 2, -m_size.y / 2, 0.0F);
		m_vertexTypes[3].texture = XMFLOAT2(m_bFlipH ? 0.0F : 1.0F, m_bFlipV ? 0.0F : 1.0F);
		m_vertexTypes[3].color = XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F);
		GL::GetAPI().glBindBuffer(GL_ARRAY_BUFFER, m_vertexID);
		GLenum myerror = glGetError();
		GL::GetAPI().glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(VERTEXTYPE), m_vertexTypes, GL_STATIC_DRAW);
		myerror = glGetError();
		return TRUE;
	}
	BOOL GLImage2D::Initialize(GL& gl)
	{
		VERTEXTYPE vertexTypes[4];
		ZeroMemory(vertexTypes, (sizeof(VERTEXTYPE) * 4));
		GLenum myerror = glGetError();
		GL::GetAPI().glGenVertexArrays(1, &m_vertexArrayID);
		myerror = glGetError();
		GL::GetAPI().glBindVertexArray(m_vertexArrayID);
		myerror = glGetError();
		GL::GetAPI().glGenBuffers(1, &m_vertexID);
		myerror = glGetError();
		GL::GetAPI().glBindBuffer(GL_ARRAY_BUFFER, m_vertexID);
		myerror = glGetError();
		GL::GetAPI().glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(VERTEXTYPE), vertexTypes, GL_STATIC_DRAW);
		myerror = glGetError();
		GL::GetAPI().glEnableVertexAttribArray(0);
		myerror = glGetError();
		GL::GetAPI().glEnableVertexAttribArray(1);
		myerror = glGetError();
		GL::GetAPI().glEnableVertexAttribArray(2);
		myerror = glGetError();
		GL::GetAPI().glBindBuffer(GL_ARRAY_BUFFER, m_vertexID);
		myerror = glGetError();
		GL::GetAPI().glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(VERTEXTYPE), NULL);
		myerror = glGetError();
		GL::GetAPI().glBindBuffer(GL_ARRAY_BUFFER, m_vertexID);
		myerror = glGetError();
		GL::GetAPI().glVertexAttribPointer(1, 2, GL_FLOAT, FALSE, sizeof(VERTEXTYPE), (BYTE*)NULL + (3 * sizeof(FLOAT)));
		myerror = glGetError();
		GL::GetAPI().glBindBuffer(GL_ARRAY_BUFFER, m_vertexID);
		myerror = glGetError();
		GL::GetAPI().glVertexAttribPointer(2, 4, GL_FLOAT, FALSE, sizeof(VERTEXTYPE), (BYTE*)NULL + (5 * sizeof(FLOAT)));
		myerror = glGetError();
		m_indices[0] = 0;
		m_indices[1] = 1;
		m_indices[2] = 3;
		m_indices[3] = 1;
		m_indices[4] = 2;
		m_indices[5] = 3;
		GL::GetAPI().glGenBuffers(1, &m_indexID);
		myerror = glGetError();
		GL::GetAPI().glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexID);
		myerror = glGetError();
		GL::GetAPI().glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(UINT), m_indices, GL_STATIC_DRAW);
		myerror = glGetError();
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

	BOOL GLImage2D::DrawImage(GL& gl)
	{
		if (!Calculate(gl))
			return FALSE;
		GL::GetAPI().glBindVertexArray(m_vertexArrayID);
		GLenum myerror = glGetError();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		myerror = glGetError();
		return TRUE;
	}

	void GLImage2D::Destory()
	{
		GLTexture2D::Destory();
		GL::GetAPI().glDisableVertexAttribArray(0);
		GL::GetAPI().glDisableVertexAttribArray(1);
		GL::GetAPI().glBindBuffer(GL_ARRAY_BUFFER, 0);
		GL::GetAPI().glDeleteBuffers(1, &m_vertexID);
		GL::GetAPI().glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		GL::GetAPI().glDeleteBuffers(1, &m_indexID);
		GL::GetAPI().glBindVertexArray(0);
		GL::GetAPI().glDeleteVertexArrays(1, &m_vertexArrayID);
	}
}