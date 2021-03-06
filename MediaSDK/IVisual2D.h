#pragma once
#include "MediaSDK.h"

namespace MediaSDK
{
	class IVisual2D :public TinyRectTracker, public TinyObject
	{
		DECLARE_DYNAMIC(IVisual2D)
		DISALLOW_COPY_AND_ASSIGN(IVisual2D)
	public:
		IVisual2D();
		virtual	~IVisual2D();
		void OnChangedRect(const TinyRectangle& rectOld) OVERRIDE;
	public:
		virtual BOOL			Open() = 0;
		virtual BOOL			Tick(INT64& timestamp) = 0;
		virtual BOOL			Draw(DX11Graphics2D& g) = 0;
		virtual void			Close() = 0;
		virtual	LPCSTR			GetName() = 0;
		virtual XMFLOAT2		GetSize() = 0;
		virtual XMFLOAT2		GetTranslate() = 0;
		virtual XMFLOAT2		GetScale() = 0;
		virtual	void			SetName(LPCSTR pzName) = 0;
		virtual	void			SetTranslate(const XMFLOAT2& pos) = 0;
		virtual	void			SetScale(const XMFLOAT2& pos) = 0;
	};
}

