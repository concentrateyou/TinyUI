#pragma once
#include "MediaSDK.h"
#include "IVisual2D.h"

namespace MediaSDK
{
	class WindowVisual2D : public IVisual2D
	{
		DECLARE_DYNAMIC(WindowVisual2D)
		DISALLOW_COPY_AND_ASSIGN(WindowVisual2D)
	public:
		WindowVisual2D(DX11& dx11);
		virtual ~WindowVisual2D();
	public:
		void			SetCompatible(BOOL bCompatibility);
		void			Select(const TinyString& className, const TinyString& exeName);
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
		void			UpdateRectangle(const XMFLOAT2& pos, const XMFLOAT2& size);
		static BOOL CALLBACK	EnumWindow(HWND hwnd, LPARAM lParam);
	private:
		BOOL			m_bCompatibility;
		HBITMAP			m_hBitmaps[2];
		HDC				m_windowDC;
		WNDINFO			m_target;
		DX11&			m_dx11;
		DX11Image2D		m_visual2D;
		DX11Cursor2D	m_cursor2D;
		TinyString		m_szNAME;
		TinySize		m_size;
	};
}

