#pragma once
#include "Common/TinyModule.h"
#include "DX11.h"
#include "DX11CaptureRunner.h"
#include "DX11Image2D.h"
using namespace TinyFramework;
using namespace DXFramework;

namespace Bytedance
{
	class IVisual
	{
		DISALLOW_COPY_AND_ASSIGN(IVisual)
	public:
		IVisual();
		virtual ~IVisual();
		virtual	LPCSTR				Name() = 0;
		virtual BOOL				Open() = 0;
		virtual BOOL				Process() = 0;
		virtual BOOL				Close() = 0;
		virtual DX11ImageElement2D*	visual() = 0;
	};
}


