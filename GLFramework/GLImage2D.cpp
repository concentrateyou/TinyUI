#include "stdafx.h"
#include "GLImage2D.h"

namespace GLFramework
{
	IMPLEMENT_DYNAMIC(GLImage2D, GLImageElement2D);

	GLImage2D::GLImage2D()
		:m_vertexArrayID(NULL),
		m_vertexID(NULL),
		m_indexID(NULL)
	{
	}

	GLImage2D::~GLImage2D()
	{
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
		GLTexture2D::Destory();
		GL::GetAPI().glDisableVertexAttribArray(0);
		GL::GetAPI().glDisableVertexAttribArray(1);
		GL::GetAPI().glDisableVertexAttribArray(2);
		GL::GetAPI().glBindBuffer(GL_ARRAY_BUFFER, 0);
		GL::GetAPI().glDeleteBuffers(1, &m_vertexID);
		GL::GetAPI().glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		GL::GetAPI().glDeleteBuffers(1, &m_indexID);
		GL::GetAPI().glBindVertexArray(0);
		GL::GetAPI().glDeleteVertexArrays(1, &m_vertexArrayID);
		GL_CHECK_ERROR();
	}

	BOOL GLImage2D::Initialize(GL& gl)
	{
		UNUSED(gl);
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

	BOOL GLImage2D::Calculate(GL& gl)
	{
		VERTEXTYPE vertices[4];
		vertices[0].position = XMFLOAT3(-1.0F * (m_size.x / 2), -1.0F * (m_size.y / 2), 0.0F);
		vertices[0].texture = XMFLOAT2(m_bFlipH ? 1.0F : 0.0F, m_bFlipV ? 0.0F : 1.0F);
		vertices[0].color = XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F);
		vertices[1].position = XMFLOAT3(-1.0F * (m_size.x / 2), 1.0F *  (m_size.y / 2), 0.0F);
		vertices[1].texture = XMFLOAT2(m_bFlipH ? 1.0F : 0.0F, m_bFlipV ? 1.0F : 0.0F);
		vertices[1].color = XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F);
		vertices[2].position = XMFLOAT3(1.0F * (m_size.x / 2), 1.0F *  (m_size.y / 2), 0.0F);
		vertices[2].texture = XMFLOAT2(m_bFlipH ? 0.0F : 1.0F, m_bFlipV ? 1.0F : 0.0F);
		vertices[2].color = XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F);
		vertices[3].position = XMFLOAT3(1.0F * (m_size.x / 2), -1.0F *  (m_size.y / 2), 0.0F);
		vertices[3].texture = XMFLOAT2(m_bFlipH ? 0.0F : 1.0F, m_bFlipV ? 0.0F : 1.0F);
		vertices[3].color = XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F);
		XMFLOAT2 size(m_size.x * m_scale.x, m_size.y * m_scale.y);
		XMFLOAT2 viewsize = gl.GetSize();
		XMFLOAT2 center(viewsize.x / 2, viewsize.y / 2);
		XMMATRIX scaling = XMMatrixScaling(m_scale.x, m_scale.y, 0.0F);
		XMMATRIX rotation = XMMatrixRotationZ(((m_angle) * (D3DX_PI / 180.0)));
		XMMATRIX translation = XMMatrixTranslation(-center.x + size.x / 2 + m_translate.x, center.y - size.y / 2 - m_translate.y, 0.0F);
		XMMATRIX* ms = gl.GetMatrixs();
		ms[1] *= scaling * rotation * translation;
		GL::GetAPI().glBindBuffer(GL_ARRAY_BUFFER, m_vertexID);
		GL::GetAPI().glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(VERTEXTYPE), vertices, GL_STATIC_DRAW);
		GL_CHECK_ERROR(FALSE);
		return TRUE;
	}

	BOOL GLImage2D::DrawImage(GL& gl)
	{
		if (!Calculate(gl))
			return FALSE;
		GL::GetAPI().glBindVertexArray(m_vertexArrayID);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		GL_CHECK_ERROR(FALSE);
		return TRUE;
	}
}