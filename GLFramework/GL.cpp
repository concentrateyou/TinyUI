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

	BOOL GL::InitializeExtensions()
	{
		HGLRC context = NULL;
		BOOL bRes = TRUE;
		TinyWindow window;
		if (!window.Create(NULL, 0, 0, 1, 1))
			return FALSE;
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
		if (!gladLoadWGL(dc))
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
