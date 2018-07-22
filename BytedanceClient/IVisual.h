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
		virtual BOOL			Open() = 0;
		virtual BOOL			Process() = 0;
		virtual BOOL			Close() = 0;
		virtual	LPCSTR			GetName() = 0;
		virtual DX11Element*	GetVisual() = 0;
	};
}


