#pragma once
#include "GLFramework.h"

namespace GLFramework
{
	class GLRenderView;
	/// <summary>
	/// OpenGL 4.0
	/// </summary>
	class GL
	{
		DISALLOW_COPY_AND_ASSIGN(GL)
	public:
		PFNGLGETERRORPROC				glGetError;
		PFNWGLCHOOSEPIXELFORMATARBPROC	wglChoosePixelFormatARB;
		PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
		PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
		PFNGLATTACHSHADERPROC glAttachShader;
		PFNGLBINDBUFFERPROC glBindBuffer;
		PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
		PFNGLBUFFERDATAPROC glBufferData;
		PFNGLCOMPILESHADERPROC glCompileShader;
		PFNGLCREATEPROGRAMPROC glCreateProgram;
		PFNGLCREATESHADERPROC glCreateShader;
		PFNGLDELETEBUFFERSPROC glDeleteBuffers;
		PFNGLDELETEPROGRAMPROC glDeleteProgram;
		PFNGLDELETESHADERPROC glDeleteShader;
		PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
		PFNGLDETACHSHADERPROC glDetachShader;
		PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
		PFNGLGENBUFFERSPROC glGenBuffers;
		PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
		PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
		PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
		PFNGLGETPROGRAMIVPROC glGetProgramiv;
		PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
		PFNGLGETSHADERIVPROC glGetShaderiv;
		PFNGLLINKPROGRAMPROC glLinkProgram;
		PFNGLSHADERSOURCEPROC glShaderSource;
		PFNGLUSEPROGRAMPROC glUseProgram;
		PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
		PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
		PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
		PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
		PFNGLACTIVETEXTUREPROC glActiveTexture;
		PFNGLUNIFORM1IPROC glUniform1i;
		PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
		PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
		PFNGLUNIFORM3FVPROC glUniform3fv;
		PFNGLUNIFORM4FVPROC glUniform4fv;
	public:
		GL();
		virtual ~GL();
		BOOL		IsEmpty() const;
		BOOL		IsActive() const;
		HDC			GetDC() const;
		HWND		GetHWND() const;
		HGLRC		GetContext() const;
		BOOL		Initialize(HWND hWND, INT cx, INT cy);
		void		Uninitialize();
		void		SetMatrixs(const TinySize& size);
		XMMATRIX*	GetMatrixs();
	private:
		BOOL		InitializeExtensions();
		BOOL		GetExtensions();
	private:
		HDC					m_hDC;
		HWND				m_hWND;
		HGLRC				m_context;
		XMMATRIX			m_matrixs[3];
		GLRenderView*		m_render2D;
	};
}
