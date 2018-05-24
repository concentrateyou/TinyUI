#pragma once
#include "GLFramework.h"

namespace GLFramework
{
	class OGL
	{
		DISALLOW_COPY_AND_ASSIGN(OGL)
	public:
		OGL();
		~OGL();
	public:
		BOOL	Initialize(HWND hWND, INT cx, INT cy);
		void	Uninitialize();
	private:
		BOOL	GetPixelFormat(INT& index, PIXELFORMATDESCRIPTOR& s);
	};
}



