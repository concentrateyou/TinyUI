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
		void				Select(const TinyString& className, const TinyString& exeName, FLOAT fFPS = 30.0F);
	public:
		BOOL				Open() OVERRIDE;
		BOOL				Tick(INT64& timestamp) OVERRIDE;
		BOOL				Draw(DX11Graphics2D& g) OVERRIDE;
		void				Close() OVERRIDE;
		LPCSTR				GetName() OVERRIDE;
		XMFLOAT2			GetTranslate() OVERRIDE;
		XMFLOAT2			GetScale() OVERRIDE;
		XMFLOAT2			GetSize() OVERRIDE;
		void				SetName(LPCSTR pzName) OVERRIDE;
		void				SetScale(const XMFLOAT2& pos) OVERRIDE;
		void				SetTranslate(const XMFLOAT2& pos) OVERRIDE;
	private:
		void				UpdateRectangle(const XMFLOAT2& pos, const XMFLOAT2& size);
		void				OnCallback();
	private:
		DX11&				m_dx11;
		DX11Image2D			m_visual2D;
		DX11CaptureRunner	m_captureRunner;
		TinyString			m_szNAME;
	};
}


