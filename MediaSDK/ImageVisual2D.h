#pragma once
#include "MediaSDK.h"
#include "IVisual2D.h"

namespace MediaSDK
{
	class ImageVisual2D : public IVisual2D
	{
		DECLARE_DYNAMIC(ImageVisual2D)
		DISALLOW_COPY_AND_ASSIGN(ImageVisual2D)
	public:
		ImageVisual2D(DX11& dx11);
		virtual ~ImageVisual2D();
	public:
		void			SetFile(const TinyString& szFile);
	public:
		BOOL			Open() OVERRIDE;
		BOOL			Tick(INT64& timestamp) OVERRIDE;
		BOOL			Draw(DX11Graphics2D& graphice2D) OVERRIDE;
		void			Close() OVERRIDE;
		LPCSTR			GetName() OVERRIDE;
		XMFLOAT2		GetTranslate() OVERRIDE;
		XMFLOAT2		GetSize() OVERRIDE;
		XMFLOAT2		GetScale() OVERRIDE;
		void			SetName(LPCSTR pzName) OVERRIDE;
		void			SetTranslate(const XMFLOAT2& pos) OVERRIDE;
		void			SetScale(const XMFLOAT2& pos) OVERRIDE;
	private:
		TinyString		m_szFile;
		TinyString		m_szNAME;
		DX11&			m_dx11;
		DX11Image2D		m_visual2D;
	};
}

