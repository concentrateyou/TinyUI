#include "stdafx.h"
#include "GL.h"

namespace GLFramework
{

	BOOL GLAPI::LoadAPI()
	{
		BOOL	bRes = TRUE;
		HDC		hDC = NULL;
		HGLRC	hGLRC = NULL;
		PIXELFORMATDESCRIPTOR pixelFormat;
		TinyWindow window;
		if (!window.Create(NULL, 0, 0, 1, 1))
		{
			bRes = FALSE;
			goto _ERROR;
		}
		hDC = GetDC(window.Handle());
		bRes = SetPixelFormat(hDC, 1, &pixelFormat);
		if (!bRes)
			goto _ERROR;
		hGLRC = wglCreateContext(hDC);
		if (!hGLRC)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		bRes = wglMakeCurrent(hDC, hGLRC);
		if (bRes != 1)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
		if (!wglChoosePixelFormatARB)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
		if (!wglCreateContextAttribsARB)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
		if (!wglSwapIntervalEXT)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
		if (!glAttachShader)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
		if (!glBindBuffer)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
		if (!glBindVertexArray)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
		if (!glBufferData)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
		if (!glCompileShader)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
		if (!glCompileShader)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
		if (!glCreateShader)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
		if (!glDeleteBuffers)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
		if (!glDeleteBuffers)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
		if (!glDeleteBuffers)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)wglGetProcAddress("glDeleteVertexArrays");
		if (!glDeleteBuffers)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		glDetachShader = (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader");
		if (!glDeleteBuffers)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
		if (!glDeleteBuffers)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
		if (!glGenBuffers)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
		if (!glGenVertexArrays)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)wglGetProcAddress("glGetAttribLocation");
		if (!glGetAttribLocation)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog");
		if (!glGetProgramInfoLog)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
		if (!glGetProgramiv)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
		if (!glGetShaderInfoLog)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
		if (!glGetShaderiv)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
		if (!glLinkProgram)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
		if (!glLinkProgram)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
		if (!glUseProgram)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
		if (!glUseProgram)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)wglGetProcAddress("glBindAttribLocation");
		if (!glUseProgram)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
		if (!glGetUniformLocation)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");
		if (!glUniformMatrix4fv)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		glActiveTexture = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
		if (!glActiveTexture)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
		if (!glUniform1i)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");
		if (!glGenerateMipmap)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glDisableVertexAttribArray");
		if (!glDisableVertexAttribArray)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		glUniform3fv = (PFNGLUNIFORM3FVPROC)wglGetProcAddress("glUniform3fv");
		if (!glUniform3fv)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		glUniform4fv = (PFNGLUNIFORM4FVPROC)wglGetProcAddress("glUniform4fv");
		if (!glUniform4fv)
		{
			bRes = FALSE;
			goto _ERROR;
		}
	_ERROR:
		wglMakeCurrent(NULL, NULL);
		if (hGLRC != NULL)
		{
			wglDeleteContext(hGLRC);
			hGLRC = NULL;
		}
		if (hDC != NULL)
		{
			ReleaseDC(window.Handle(), hDC);
			hDC = NULL;
		}
		window.DestroyWindow();
		return bRes;
	};

	GL::GL()
		:m_hDC(NULL),
		m_hWND(NULL),
		m_context(NULL)
	{
	}

	GL::~GL()
	{
	}
	GLAPI& GL::GetAPI()
	{
		static GLAPI api;
		return api;
	}
	BOOL GL::IsEmpty() const
	{
		return (m_hDC == NULL);
	}
	BOOL GL::IsActive() const
	{
		return wglGetCurrentContext() == m_context;
	}
	HDC	GL::GetDC() const
	{
		return m_hDC;
	}

	HWND GL::GetHWND() const
	{
		return m_hWND;
	}

	HGLRC GL::GetContext() const
	{
		return m_context;
	}

	BOOL GL::Initialize(HWND hWND, INT cx, INT cy)
	{
		if (!GetAPI().LoadAPI())
			return FALSE;
		const INT attribs[] =
		{
			WGL_SUPPORT_OPENGL_ARB,
			TRUE,
			WGL_DRAW_TO_WINDOW_ARB,
			TRUE,
			WGL_ACCELERATION_ARB,
			WGL_FULL_ACCELERATION_ARB,
			WGL_COLOR_BITS_ARB,
			32,
			WGL_DEPTH_BITS_ARB,
			24,
			WGL_DOUBLE_BUFFER_ARB,
			TRUE,
			WGL_SWAP_METHOD_ARB,
			WGL_SWAP_EXCHANGE_ARB,
			WGL_PIXEL_TYPE_ARB,
			WGL_TYPE_RGBA_ARB,
			WGL_STENCIL_BITS_ARB,
			8,
			0
		};
		const INT contextAttribs[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB,
			4,
			WGL_CONTEXT_MINOR_VERSION_ARB,
			0,
			0
		};
		INT iFormat = 1;
		UINT count = 0;
		PIXELFORMATDESCRIPTOR sPFD;
		ZeroMemory(&sPFD, sizeof(sPFD));
		sPFD.nSize = sizeof(sPFD);
		m_hWND = hWND;
		m_hDC = ::GetDC(m_hWND);
		if (!m_hDC)
			goto _ERROR;
		if (!GetAPI().wglChoosePixelFormatARB(m_hDC, attribs, NULL, 1, &iFormat, &count))
			goto _ERROR;
		if (!SetPixelFormat(m_hDC, iFormat, &sPFD))
			goto _ERROR;
		m_context = GetAPI().wglCreateContextAttribsARB(m_hDC, 0, contextAttribs);
		if (!m_context)
			goto _ERROR;
		if (!wglMakeCurrent(m_hDC, m_context))
			goto _ERROR;
		glClearDepth(1.0F);
		glEnable(GL_DEPTH_TEST);
		glFrontFace(GL_CW);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		Resize(cx, cy);
		GetAPI().wglSwapIntervalEXT(0);//关闭垂直同步
		m_hWND = hWND;
		return TRUE;
	_ERROR:
		Uninitialize();
		return FALSE;
	}

	void GL::Uninitialize()
	{
		wglMakeCurrent(NULL, NULL);
		if (m_context != NULL)
		{
			wglDeleteContext(m_context);
			m_context = NULL;
		}
		if (m_hDC != NULL && m_hWND != NULL)
		{
			ReleaseDC(m_hWND, m_hDC);
			m_hDC = NULL;
			m_hWND = NULL;
		}
	}
	void GL::Resize(INT cx, INT cy)
	{
		glViewport(0, 0, cx, cy);
		SetMatrixs(TinySize(cx, cy));
	}
	void GL::SetMatrixs(const TinySize& size)
	{
		FLOAT fov = (FLOAT)D3DX_PI / 4.0F;
		FLOAT aspect = (FLOAT)size.cx / (FLOAT)size.cy;
		m_matrixs[0] = XMMatrixPerspectiveFovLH(fov, aspect, 1000.0F, 0.1F);//projection
		m_matrixs[1] = XMMatrixIdentity();//world
		m_matrixs[2] = XMMatrixOrthographicLH((FLOAT)size.cx, (FLOAT)size.cy, 1000.0F, 0.1F);//view
	}
	XMMATRIX* GL::GetMatrixs()
	{
		return m_matrixs;
	}
}