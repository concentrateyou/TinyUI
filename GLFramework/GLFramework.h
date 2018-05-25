#pragma once
#include "Common/TinyCommon.h"
#include "Common/TinyUtility.h"
#include "Common/TinyTime.h"
#include "Common/TinyLogging.h"
#include "Common/TinyObject.h"
#include "Control/TinyControl.h"
#include "Render/TinyImage.h"
#include "IO/TinyIO.h"
#include "glad/glad_wgl.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#pragma comment(lib,"opengl32.lib")

using namespace TinyUI;
using namespace TinyUI::IO;
using namespace std;

#define GL_PI    (3.14159265358979323846)