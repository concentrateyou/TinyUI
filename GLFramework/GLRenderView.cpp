#include "stdafx.h"
#include "GLRenderView.h"

namespace GLFramework
{
	GLRenderView::GLRenderView(GL& gl)
		:m_gl(gl),
		m_frameBuffers(0),
		m_depthBuffers(0),
		m_textureID(0)
	{
	}

	GLRenderView::~GLRenderView()
	{
	}
	GL&	GLRenderView::GetGL()
	{
		return m_gl;
	}
	BOOL GLRenderView::Create(INT cx, INT cy)
	{
		glClearErrors();
		glGenFramebuffers(1, &m_frameBuffers);
		GL_CHECK_ERROR(FALSE);
		glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffers);
		GL_CHECK_ERROR(FALSE);
		glGenTextures(1, &m_textureID);
		GL_CHECK_ERROR(FALSE);
		glBindTexture(GL_TEXTURE_2D, m_textureID);
		GL_CHECK_ERROR(FALSE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cx, cy, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
		GL_CHECK_ERROR(FALSE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		GL_CHECK_ERROR(FALSE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		GL_CHECK_ERROR(FALSE);
		glGenRenderbuffers(1, &m_depthBuffers);
		GL_CHECK_ERROR(FALSE);
		glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffers);
		GL_CHECK_ERROR(FALSE);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, cx, cy);
		GL_CHECK_ERROR(FALSE);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffers);
		GL_CHECK_ERROR(FALSE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureID, 0);
		GL_CHECK_ERROR(FALSE);
		GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, drawBuffers);
		GL_CHECK_ERROR(FALSE);
		return TRUE;
	}
	BOOL GLRenderView::Resize(INT cx, INT cy)
	{
		return TRUE;
	}
	void GLRenderView::Destory()
	{
		if (glIsRenderbuffer(m_depthBuffers))
		{
			glDeleteRenderbuffers(1, &m_depthBuffers);
		}
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		if (glIsTexture(m_textureID))
		{
			glDeleteTextures(1, &m_textureID);
		}
		if (glIsFramebuffer(m_frameBuffers))
		{
			glDeleteFramebuffers(1, &m_frameBuffers);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}
	BOOL GLRenderView::BeginDraw()
	{
		glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		return TRUE;
	}
	BOOL GLRenderView::EndDraw()
	{
		return SwapBuffers(m_gl.GetDC());
	}
}