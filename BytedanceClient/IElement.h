#pragma once
#include "Common/TinyModule.h"
#include "DX11.h"
#include "DX11CaptureRunner.h"
#include "DX11Image2D.h"
using namespace TinyFramework;
using namespace DXFramework;

namespace Bytedance
{
	class IElement
	{
		DISALLOW_COPY_AND_ASSIGN(IElement)
	public:
		IElement();
		virtual ~IElement();
		virtual BOOL				Open() = 0;
		virtual BOOL				Process() = 0;
		virtual BOOL				Close() = 0;
		virtual	LPCSTR				GetName() = 0;
		virtual DX11ImageElement2D*	GetVisual() = 0;
	};
}


