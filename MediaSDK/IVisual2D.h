#pragma once
#include "MediaSDK.h"

namespace MediaSDK
{
	class IVisual2D
	{
		DISALLOW_COPY_AND_ASSIGN(IVisual2D)
	public:
		IVisual2D();
		virtual	~IVisual2D();
	public:
		virtual BOOL			Open() = 0;
		virtual BOOL			Process() = 0;
		virtual BOOL			Close() = 0;
		virtual	LPCSTR			GetName() = 0;
		virtual DX11Image2D*	GetVisual2D() = 0;
	};
}

