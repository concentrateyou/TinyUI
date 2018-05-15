#include "stdafx.h"
#include "GL.h"

namespace GLFramework
{
	GL::GL()
		:m_hDC(NULL),
		m_hWND(NULL),
		m_context(NULL),
		m_render2D(NULL)
	{
		wglChoosePixelFormatARB = NULL;
		wglCreateContextAttribsARB = NULL;
		wglSwapIntervalEXT = NULL;
		glAttachShader = NULL;
		glBindBuffer = NULL;
		glBindVertexArray = NULL;
		glBufferData = NULL;
		glCompileShader = NULL;
		glCreateProgram = NULL;
		glCreateShader = NULL;
		glDeleteBuffers = NULL;
		glDeleteProgram = NULL;
		glDeleteShader = NULL;
		glDeleteVertexArrays = NULL;
		glDetachShader = NULL;
		glEnableVertexAttribArray = NULL;
		glGenBuffers = NULL;
		glGenVertexArrays = NULL;
		glGetAttribLocation = NULL;
		glGetProgramInfoLog = NULL;
		glGetProgramiv = NULL;
		glGetShaderInfoLog = NULL;
		glGetShaderiv = NULL;
		glLinkProgram = NULL;
		glShaderSource = NULL;
		glUseProgram = NULL;
		glVertexAttribPointer = NULL;
		glBindAttribLocation = NULL;
		glGetUniformLocation = NULL;
		glUniformMatrix4fv = NULL;
		glActiveTexture = NULL;
		glUniform1i = NULL;
		glGenerateMipmap = NULL;
		glDisableVertexAttribArray = NULL;
		glUniform3fv = NULL;
		glUniform4fv = NULL;
	}

	GL::~GL()
	{
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

	BOOL GL::GetExtensions()
	{
		glGetError = (PFNGLGETERRORPROC)wglGetProcAddress("glGetError");
		if (!glGetError)
			return FALSE;
		wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
		if (!wglChoosePixelFormatARB)
			return FALSE;
		wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
		if (!wglCreateContextAttribsARB)
			return FALSE;
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
		if (!wglSwapIntervalEXT)
			return FALSE;
		glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
		if (!glAttachShader)
			return FALSE;
		glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
		if (!glBindBuffer)
			return FALSE;
		glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
		if (!glBindVertexArray)
			return FALSE;
		glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
		if (!glBufferData)
			return FALSE;
		glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
		if (!glCompileShader)
			return FALSE;
		glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
		if (!glCreateProgram)
			return FALSE;
		glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
		if (!glCreateShader)
			return FALSE;
		glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
		if (!glDeleteBuffers)
			return FALSE;
		glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
		if (!glDeleteProgram)
			return FALSE;
		glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
		if (!glDeleteShader)
			return FALSE;
		glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)wglGetProcAddress("glDeleteVertexArrays");
		if (!glDeleteVertexArrays)
			return FALSE;
		glDetachShader = (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader");
		if (!glDetachShader)
			return FALSE;
		glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
		if (!glEnableVertexAttribArray)
			return FALSE;
		glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
		if (!glGenBuffers)
			return FALSE;
		glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
		if (!glGenVertexArrays)
			return FALSE;
		glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)wglGetProcAddress("glGetAttribLocation");
		if (!glGetAttribLocation)
			return FALSE;
		glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog");
		if (!glGetProgramInfoLog)
			return FALSE;
		glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
		if (!glGetProgramiv)
			return FALSE;
		glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
		if (!glGetShaderInfoLog)
			return FALSE;
		glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
		if (!glGetShaderiv)
			return FALSE;
		glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
		if (!glLinkProgram)
			return FALSE;
		glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
		if (!glShaderSource)
			return FALSE;
		glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
		if (!glUseProgram)
			return FALSE;
		glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
		if (!glVertexAttribPointer)
			return FALSE;
		glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)wglGetProcAddress("glBindAttribLocation");
		if (!glBindAttribLocation)
			return FALSE;
		glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
		if (!glGetUniformLocation)
			return FALSE;
		glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");
		if (!glUniformMatrix4fv)
			return FALSE;
		glActiveTexture = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
		if (!glActiveTexture)
			return FALSE;
		glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
		if (!glUniform1i)
			return FALSE;
		glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");
		if (!glGenerateMipmap)
			return FALSE;
		glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glDisableVertexAttribArray");
		if (!glDisableVertexAttribArray)
			return FALSE;
		glUniform3fv = (PFNGLUNIFORM3FVPROC)wglGetProcAddress("glUniform3fv");
		if (!glUniform3fv)
			return FALSE;
		glUniform4fv = (PFNGLUNIFORM4FVPROC)wglGetProcAddress("glUniform4fv");
		if (!glUniform4fv)
			return FALSE;
		return TRUE;
	}

	BOOL GL::InitializeExtensions()
	{
		BOOL bRes = TRUE;
		TinyWindow window;
		if (!window.Create(NULL, 0, 0, 1, 1))
			return FALSE;
		HGLRC context = NULL;
		TinyClientDC dc(window.Handle());
		PIXELFORMATDESCRIPTOR sPFD;
		ZeroMemory(&sPFD, sizeof(sPFD));
		sPFD.nSize = sizeof(sPFD);
		sPFD.nVersion = 1;
		sPFD.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		sPFD.iPixelType = PFD_TYPE_RGBA;
		sPFD.cColorBits = 32;
		sPFD.cAlphaBits = 8;
		sPFD.cDepthBits = 24;
		INT iFormat = ChoosePixelFormat(dc, &sPFD);
		if (!iFormat)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		if (!SetPixelFormat(dc, iFormat, &sPFD))
		{
			bRes = FALSE;
			goto _ERROR;
		}
		context = wglCreateContext(dc);
		if (!context)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		if (!wglMakeCurrent(dc, context))
		{
			bRes = FALSE;
			goto _ERROR;
		}
		if (!GetExtensions())
		{
			bRes = FALSE;
			goto _ERROR;
		}
	_ERROR:
		window.DestroyWindow();
		wglMakeCurrent(NULL, NULL);
		if (context != NULL)
		{
			wglDeleteContext(context);
			context = NULL;
		}
		return bRes;
	}

	BOOL GL::Initialize(HWND hWND, INT cx, INT cy)
	{
		if (!InitializeExtensions())
			return FALSE;
		const INT attribs[] = {
			WGL_DRAW_TO_WINDOW_ARB,
			GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB,
			GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB,
			GL_TRUE,
			WGL_PIXEL_TYPE_ARB,
			WGL_TYPE_RGBA_ARB,
			WGL_ACCELERATION_ARB,
			WGL_FULL_ACCELERATION_ARB,
			WGL_COLOR_BITS_ARB,
			32,
			WGL_ALPHA_BITS_ARB,
			8,
			WGL_DEPTH_BITS_ARB,
			24,
			WGL_STENCIL_BITS_ARB,
			8,
			WGL_SAMPLE_BUFFERS_ARB,
			GL_TRUE,
			WGL_SAMPLES_ARB,
			4,
			0
		};
		const INT contextAttribs[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
			WGL_CONTEXT_MINOR_VERSION_ARB, 0,
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			0
		};
		INT iFormat = 1;
		UINT count = 0;
		PIXELFORMATDESCRIPTOR sPFD;
		ZeroMemory(&sPFD, sizeof(sPFD));
		sPFD.nSize = sizeof(sPFD);
		m_hDC = ::GetDC(m_hWND);
		if (!m_hDC)
			goto _ERROR;
		if (!wglChoosePixelFormatARB(m_hDC, attribs, NULL, 1, &iFormat, &count))
			goto _ERROR;
		if (!DescribePixelFormat(m_hDC, iFormat, sizeof(sPFD), &sPFD))
			goto _ERROR;
		if (!SetPixelFormat(m_hDC, iFormat, &sPFD))
			goto _ERROR;
		m_context = wglCreateContextAttribsARB(m_hDC, 0, contextAttribs);
		if (!m_context)
			goto _ERROR;
		if (!wglMakeCurrent(m_hDC, m_context))
			goto _ERROR;
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glClearDepth(1.0F);
		glFrontFace(GL_CW);
		glCullFace(GL_BACK);
		SetMatrixs(TinySize(cx, cy));
		wglSwapIntervalEXT(0);//关闭垂直同步
		m_hWND = hWND;
		return TRUE;
	_ERROR:
		wglMakeCurrent(NULL, NULL);
		if (m_context != NULL)
		{
			wglDeleteContext(m_context);
			m_context = NULL;
		}
		if (m_hDC != NULL)
		{
			::ReleaseDC(m_hWND, m_hDC);
			m_hDC = NULL;
		}
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
