#pragma once
#include "DX11Element2D.h"
#include "DX11Texture2D.h"
#include "DX11TextureShader.h"
#include "Render/TinyGDI.h"
using namespace TinyUI;

namespace DXFramework
{
	class DX11ImageNV12 : public DX11Element2D, public DX11Texture2D
	{
		DISALLOW_COPY_AND_ASSIGN(DX11ImageNV12)
	public:
		DX11ImageNV12();
		virtual ~DX11ImageNV12();
	};
}


