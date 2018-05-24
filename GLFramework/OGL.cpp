#include "stdafx.h"
#include "OGL.h"

namespace GLFramework
{
	OGL::OGL()
	{
	}

	OGL::~OGL()
	{
	}

	BOOL OGL::Initialize(HWND hWND, INT cx, INT cy)
	{
		INT index = 0;
		PIXELFORMATDESCRIPTOR s;
		if (!GetPixelFormat(index, s))
			return FALSE;
		return TRUE;
	}
	void OGL::Uninitialize()
	{

	}
	BOOL OGL::GetPixelFormat(INT& index, PIXELFORMATDESCRIPTOR& s)
	{
		BOOL bRes = TRUE;
		TinyWindow window;
		if (!window.Create(NULL, 0, 0, 1, 1))
			return FALSE;
		TinyClientDC dc(window);
		ZeroMemory(&s, sizeof(s));
		s.nSize = sizeof(s);
		s.nVersion = 1;
		s.iPixelType = PFD_TYPE_RGBA;
		s.cColorBits = 32;
		s.cDepthBits = 24;
		s.cStencilBits = 8;
		s.cAlphaBits = 8;
		s.iLayerType = PFD_MAIN_PLANE;
		s.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		INT value = ChoosePixelFormat(dc, &s);
		if (!value)
		{
			TRACE("ChoosePixelFormat Error:%lu\n", GetLastError());
			LOG(ERROR) << "ChoosePixelFormat Error" << GetLastError();
			return FALSE;
		}
		if (!SetPixelFormat(dc, index, &s))
		{
			TRACE("SetPixelFormat Error:%lu\n", GetLastError());
			LOG(ERROR) << "SetPixelFormat Error" << GetLastError();
			return FALSE;
		}
		HGLRC hGLRC = wglCreateContext(dc);
		if (!hGLRC)
		{
			TRACE("wglCreateContext Error:%lu\n", GetLastError());
			LOG(ERROR) << "wglCreateContext Error" << GetLastError();
			return FALSE;
		}
		if (!wglMakeCurrent(dc, hGLRC))
		{
			wglDeleteContext(hGLRC);
			TRACE("wglMakeCurrent Error:%lu\n", GetLastError());
			LOG(ERROR) << "wglMakeCurrent Error" << GetLastError();
			bRes = FALSE;
			goto _ERROR;
		}
		if (!gladLoadWGL(dc))
		{
			TRACE("gladLoadWGL Error\n");
			LOG(ERROR) << "gladLoadWGL Error";
			bRes = FALSE;
			goto _ERROR;
		}
		const INT attribs[] =
		{
			WGL_DRAW_TO_WINDOW_ARB,
			GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB,
			GL_TRUE,
			WGL_ACCELERATION_ARB,
			WGL_FULL_ACCELERATION_ARB,
			WGL_PIXEL_TYPE_ARB,
			WGL_TYPE_RGBA_ARB,
			WGL_COLOR_BITS_ARB,
			32,
			WGL_DEPTH_BITS_ARB,
			24,
			WGL_STENCIL_BITS_ARB,
			8,
			0,
			0
		};
		UINT count = 0;
		if (!wglChoosePixelFormatARB(dc, attribs, NULL, 1, &index, &count))
		{
			TRACE("wglChoosePixelFormatARB Error\n");
			LOG(ERROR) << "wglChoosePixelFormatARB Error";
			bRes = FALSE;
			goto _ERROR;
		}
		if (!DescribePixelFormat(dc, index, sizeof(s), &s))
		{
			TRACE("DescribePixelFormat Error\n");
			LOG(ERROR) << "DescribePixelFormat Error";
			bRes = FALSE;
			goto _ERROR;
		}
	_ERROR:
		if (hGLRC != NULL)
		{
			wglMakeCurrent(0, 0);
			wglDeleteContext(hGLRC);
			hGLRC = NULL;
		}
		return bRes;
	}
}