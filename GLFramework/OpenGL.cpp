#include "stdafx.h"
#include "OpenGL.h"

namespace GLFramework
{
	static void APIENTRY glDebugCallback(
		GLenum source, GLenum type, GLuint id, GLenum severity,
		GLsizei length, const GLchar *message, const GLvoid *data)
	{
		char *source_str, *type_str, *severity_str;

		if (type > GL_DEBUG_TYPE_PORTABILITY &&
			severity != GL_DEBUG_SEVERITY_HIGH)
		{
			return;
		}

		switch (source) {
		case GL_DEBUG_SOURCE_API:
			source_str = "API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			source_str = "Window System"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			source_str = "Shader Compiler"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			source_str = "Third Party"; break;
		case GL_DEBUG_SOURCE_APPLICATION:
			source_str = "Application"; break;
		case GL_DEBUG_SOURCE_OTHER:
			source_str = "Other"; break;
		default:
			source_str = "Unknown";
		}

		switch (type) {
		case GL_DEBUG_TYPE_ERROR:
			type_str = "Error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			type_str = "Deprecated Behavior"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			type_str = "Undefined Behavior"; break;
		case GL_DEBUG_TYPE_PORTABILITY:
			type_str = "Portability"; break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			type_str = "Performance"; break;
		case GL_DEBUG_TYPE_OTHER:
			type_str = "Other"; break;
		default:
			type_str = "Unknown";
		}

		switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH:
			severity_str = "High"; break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			severity_str = "Medium"; break;
		case GL_DEBUG_SEVERITY_LOW:
			severity_str = "Low"; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			severity_str = "Notification"; break;
		default:
			severity_str = "Unknown";
		}
		TRACE("[%s][%s]{%s}: %.*s",
			source_str, type_str, severity_str,
			length, message);
	}

	OpenGL::OpenGL()
		:m_hDC(NULL),
		m_hGLRC(NULL)
	{
	}

	OpenGL::~OpenGL()
	{
	}

	BOOL OpenGL::Initialize(HWND hWND, INT cx, INT cy)
	{
		INT index = 0;
		PIXELFORMATDESCRIPTOR s;
		if (!GetPixelFormat(index, s))
			return FALSE;
		m_hWND = hWND;
		m_hDC = GetDC(hWND);
		if (!m_hDC)
			return FALSE;
		if (!SetPixelFormat(m_hDC, index, &s))
			return FALSE;
		static const INT attribs[] =
		{
#ifdef _DEBUG
			WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
#endif
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			0, 0
		};
		m_hGLRC = wglCreateContextAttribsARB(m_hDC, 0, attribs);
		if (!m_hGLRC)
		{
			TRACE("wglCreateContext Error:%lu\n", GetLastError());
			LOG(ERROR) << "wglCreateContext Error" << GetLastError();
			return FALSE;
		}

		if (!wglMakeCurrent(m_hDC, m_hGLRC))
		{
			TRACE("wglMakeCurrent Error:%lu\n", GetLastError());
			LOG(ERROR) << "wglMakeCurrent Error" << GetLastError();
			goto _ERROR;
		}
		if (!gladLoadGL())
		{
			TRACE("gladLoadGL Error:%lu\n", GetLastError());
			LOG(ERROR) << "gladLoadGL Error" << GetLastError();
			goto _ERROR;
		}
		GLenum errorcode = glGetError();
		if (!GLAD_GL_VERSION_2_1)
		{
			TRACE("Requires OpenGL version 2.1 or higher\n");
			LOG(ERROR) << "Requires OpenGL version 2.1 or higher";
			return FALSE;
		}
		do
		{
			if (GLAD_GL_VERSION_4_3)
			{
				glDebugMessageCallback(glDebugCallback, NULL);
				glEnable(GL_DEBUG_OUTPUT);
				break;
			}
			if (GLAD_GL_ARB_debug_output)
			{
				glDebugMessageCallbackARB(glDebugCallback, NULL);
				break;
			}
		} while (0);
		if (!GLAD_GL_VERSION_3_0 && !GLAD_GL_ARB_framebuffer_object)
		{
			TRACE("OpenGL extension ARB_framebuffer_object is required\n");
			LOG(ERROR) << "OpenGL extension ARB_framebuffer_object is required";
			return false;
		}
		if (GLAD_GL_VERSION_3_2 || GLAD_GL_ARB_seamless_cube_map)
		{
			glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		}
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glViewport(0, 0, cx, cy);
		wglSwapIntervalEXT(0);
		return TRUE;
	_ERROR:
		Uninitialize();
		return FALSE;
	}
	void OpenGL::Uninitialize()
	{
		wglMakeCurrent(NULL, NULL);
		if (m_hGLRC != NULL)
		{
			wglDeleteContext(m_hGLRC);
			m_hGLRC = NULL;
		}
		if (m_hDC != NULL)
		{
			ReleaseDC(m_hWND, m_hDC);
			m_hDC = NULL;
		}
		m_hWND = NULL;
	}
	BOOL OpenGL::GetPixelFormat(INT& index, PIXELFORMATDESCRIPTOR& s)
	{
		BOOL bRes = TRUE;
		HGLRC hGLRC = NULL;
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
			goto _ERROR;
		}
		if (!SetPixelFormat(dc, value, &s))
		{
			TRACE("SetPixelFormat Error:%lu\n", GetLastError());
			LOG(ERROR) << "SetPixelFormat Error" << GetLastError();
			goto _ERROR;
		}
		hGLRC = wglCreateContext(dc);
		if (!hGLRC)
		{
			TRACE("wglCreateContext Error:%lu\n", GetLastError());
			LOG(ERROR) << "wglCreateContext Error" << GetLastError();
			goto _ERROR;
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
		if (!GLAD_WGL_ARB_pixel_format)
		{
			TRACE("Required ARB_pixel_format\n");
			LOG(ERROR) << "Require ARB_pixel_format";
			goto _ERROR;
		}
		if (!GLAD_WGL_ARB_create_context)
		{
			TRACE("Required GLAD_WGL_ARB_create_context\n");
			LOG(ERROR) << "Require GLAD_WGL_ARB_create_context";
			goto _ERROR;
		}
		if (!GLAD_WGL_ARB_create_context_profile)
		{
			TRACE("Required GLAD_WGL_ARB_create_context_profile\n");
			LOG(ERROR) << "Require GLAD_WGL_ARB_create_context_profile";
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
		window.DestroyWindow();
		return bRes;
	}
	BOOL OpenGL::BeginDraw()
	{
		glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
		glClearDepth(1.0F);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		return TRUE;
	}
	BOOL OpenGL::EndDraw()
	{
		BOOL bRes = SwapBuffers(m_hDC);
		return bRes;
	}
}