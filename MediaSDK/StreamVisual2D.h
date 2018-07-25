#pragma once
#include "MediaSDK.h"
#include "IVisual2D.h"

namespace MediaSDK
{
	class StreamVisual2D : public IVisual2D
	{
		DISALLOW_COPY_AND_ASSIGN(StreamVisual2D)
	public:
		StreamVisual2D(DX11& dx11);
		virtual ~StreamVisual2D();
	public:
		BOOL			Open() OVERRIDE;
		BOOL			Process() OVERRIDE;
		void			Close() OVERRIDE;
		LPCSTR			GetName() OVERRIDE;
		DX11Image2D*	GetVisual2D() OVERRIDE;
	private:
		DX11&		m_dx11;
		DX11Image2D	m_visual2D;
		TinyString	m_szURL;
	};

}


