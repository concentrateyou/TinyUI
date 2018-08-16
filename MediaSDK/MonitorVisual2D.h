#pragma once
#include "MediaSDK.h"
#include "IVisual2D.h"

namespace MediaSDK
{
	class MonitorVisual2D : public IVisual2D
	{
		DISALLOW_COPY_AND_ASSIGN(MonitorVisual2D)
	public:
		MonitorVisual2D(DX11& dx11);
		virtual	~MonitorVisual2D();
	public:
		UINT			GetOutputCount() const;
		BOOL			SetOutput(UINT index);
	public:
		BOOL			Open() OVERRIDE;
		BOOL			Tick() OVERRIDE;
		void			Close() OVERRIDE;
		LPCSTR			GetName() OVERRIDE;
		DX11Image2D*	GetVisual2D() OVERRIDE;
		XMFLOAT2		GetTranslate() OVERRIDE;
		XMFLOAT2		GetSize() OVERRIDE;
		void			SetTranslate(const XMFLOAT2& pos) OVERRIDE;
		void			SetScale(const XMFLOAT2& pos) OVERRIDE;
	private:
		DX11&			m_dx11;
		DX11Image2D		m_visual2D;
		DX11Duplicator	m_duplicator;
	};
}



