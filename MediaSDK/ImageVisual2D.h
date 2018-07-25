#pragma once
#include "MediaSDK.h"
#include "IVisual2D.h"

namespace MediaSDK
{
	class ImageVisual2D : public IVisual2D
	{
		DISALLOW_COPY_AND_ASSIGN(ImageVisual2D)
	public:
		ImageVisual2D(DX11& dx11);
		virtual ~ImageVisual2D();
	public:
		void			SetFile(const TinyString& szFile);
	public:
		BOOL			Open() OVERRIDE;
		BOOL			Process() OVERRIDE;
		void			Close() OVERRIDE;
		LPCSTR			GetName() OVERRIDE;
		DX11Image2D*	GetVisual2D() OVERRIDE;
	private:
		TinyString		m_szFile;
		DX11&			m_dx11;
		DX11Image2D		m_visual2D;
	};
}

