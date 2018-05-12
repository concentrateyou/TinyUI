#pragma once
#include "DX11Element2D.h"
#include "DX11Texture2D.h"
#include "DX11TextureShader.h"
#include "Render/TinyGDI.h"
using namespace TinyUI;

namespace DXFramework
{
	class DX11ImageYUVBT601 : public DX11Element2D, public DX11Texture2D
	{
		DISALLOW_COPY_AND_ASSIGN(DX11ImageYUVBT601)
	public:
		DX11ImageYUVBT601();
		virtual ~DX11ImageYUVBT601();
	};
}


