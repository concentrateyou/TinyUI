#pragma once
#include "Common/TinyCommon.h"
#include "Common/TinyUtility.h"
#include "Common/TinyTime.h"
#include "Common/TinyLogging.h"
#include "Common/TinyObject.h"
#include "Control/TinyControl.h"
#include "IO/TinyIO.h"
#include "glad/glad.h"
#include "glad/glad_wgl.h"
#include <DirectXMath.h>
#pragma comment(lib,"Opengl32.lib")

using namespace DirectX;
using namespace TinyUI;
using namespace TinyUI::IO;
using namespace std;

#ifndef D3DX_PI
#define	D3DX_PI	(3.14159265358979323846)
#endif        

#define glClearErrors() while(glGetError());

BOOL GLSuccess();