#pragma once
#include "MediaSDK.h"
#include "IVisual2D.h"

namespace MediaSDK
{
	class GameVisual2D : public IVisual2D
	{
		DISALLOW_COPY_AND_ASSIGN(GameVisual2D)
	public:
		GameVisual2D(DX11& dx11);
		virtual ~GameVisual2D();
		void			Select(const TinyString& className, const TinyString& exeName, const TinyString& dllName);
	public:
		BOOL			Open() OVERRIDE;
		BOOL			Tick() OVERRIDE;
		void			Close() OVERRIDE;
		LPCSTR			GetName() OVERRIDE;
		DX11Image2D*	GetVisual2D() OVERRIDE;
		XMFLOAT2		GetTranslate() OVERRIDE;
		XMFLOAT2		GetScale() OVERRIDE;
		XMFLOAT2		GetSize() OVERRIDE;
		void			SetScale(const XMFLOAT2& pos) OVERRIDE;
		void			SetTranslate(const XMFLOAT2& pos) OVERRIDE;
	private:
		DX11&				m_dx11;
		DX11Image2D			m_visual2D;
		DX11CaptureRunner	m_captureRunner;
		TinyMutex			m_mutes[2];
		BYTE*				m_textures[2];
	};
}


