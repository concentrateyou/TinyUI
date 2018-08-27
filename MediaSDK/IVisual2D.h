#pragma once
#include "MediaSDK.h"

namespace MediaSDK
{
	class IVisual2D :public TinyRectTracker
	{
		DISALLOW_COPY_AND_ASSIGN(IVisual2D)
	public:
		IVisual2D();
		virtual	~IVisual2D();
	public:
		virtual BOOL			Open() = 0;
		virtual BOOL			Tick() = 0;
		virtual void			Close() = 0;
		virtual	LPCSTR			GetVisualName() = 0;
		virtual DX11Image2D*	GetVisual2D() = 0;
		virtual XMFLOAT2		GetTranslate() = 0;
		virtual XMFLOAT2		GetScale() = 0;
		virtual XMFLOAT2		GetSize() = 0;
		virtual	void			SetTranslate(const XMFLOAT2& pos) = 0;
		virtual	void			SetScale(const XMFLOAT2& pos) = 0;
	};
}

