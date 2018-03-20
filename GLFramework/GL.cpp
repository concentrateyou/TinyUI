#include "stdafx.h"
#include "GL.h"

namespace GLFramework
{
	GL::GL()
	{
	}

	GL::~GL()
	{
	}

	BOOL GL::Initialize()
	{
		glEnable(GL_LINE_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		return TRUE;
	}

	BOOL GL::Render()
	{
		return TRUE;
	}
}
