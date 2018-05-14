#pragma once
#include "DX11ImageElement2D.h"
#include "Render/TinyGDI.h"
using namespace TinyUI;

namespace DXFramework
{
	class DX11YUVVideo : public DX11Element2D, public DX11Texture2D
	{
		DECLARE_DYNAMIC(DX11YUVVideo)
		DISALLOW_COPY_AND_ASSIGN(DX11YUVVideo)
	public:
		DX11YUVVideo();
		virtual ~DX11YUVVideo();
	};
}


