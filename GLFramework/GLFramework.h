#pragma once
#include "Common/TinyCommon.h"
#include "Common/TinyUtility.h"
#include "Common/TinyTime.h"
#include "Common/TinyLogging.h"
#include "Control/TinyControl.h"
#include "Common/TinyString.h"
#include "GL/glew.h"
#include "GL/wglew.h"
#include <DirectXMath.h>
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib,"Glu32.lib")
using namespace DirectX;
using namespace TinyUI;
using namespace std;

#ifndef D3DX_PI
#define	D3DX_PI	(3.14159265358979323846)
#endif 