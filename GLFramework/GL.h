#pragma once
#include "GLFramework.h"

namespace GLFramework
{
	class GLAPI
	{
	public:
		PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
		PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
		PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
		PFNGLATTACHSHADERPROC glAttachShader;
		PFNGLBINDBUFFERPROC glBindBuffer;
		PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
		PFNGLBUFFERDATAPROC glBufferData;
		PFNGLMAPBUFFERPROC glMapBuffer;
		PFNGLUNMAPBUFFERPROC glUnmapBuffer;
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
		BOOL	LoadAPI();
	};

	class GL
	{
		DISALLOW_COPY_AND_ASSIGN(GL)
	public:
		GL();
		~GL();
		static GLAPI& GetAPI();
	public:
		BOOL		IsEmpty() const;
		BOOL		IsActive() const;
		HDC			GetDC() const;
		HWND		GetHWND() const;
		HGLRC		GetContext() const;
		XMFLOAT2	GetSize() const;
	public:
		BOOL		Initialize(HWND hWND, INT cx, INT cy);
		void		Uninitialize();
		void		Resize(INT cx, INT cy);
		void		SetMatrixs(const XMFLOAT2& size);
		XMMATRIX*	GetMatrixs();
	private:
		HWND		m_hWND;
		HDC			m_hDC;
		HGLRC		m_context;
		XMFLOAT2	m_size;
		XMMATRIX	m_matrixs[3];
	};
}