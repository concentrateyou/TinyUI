#include "stdafx.h"
#include "GLFramework.h"

BOOL GLSuccess()
{
	GLenum code = glGetError();
	return GL_NO_ERROR == code;
}
