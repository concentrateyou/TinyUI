#pragma once
#include "Common/TinyCommon.h"
#include "DX11Image2D.h"
#include "DX11Duplicator.h"
#include "DX11.h"
#include "Utility.h"
using namespace TinyUI;
using namespace DXFramework;

namespace DXApp
{
	class ScreenScene : public DX11Image2D
	{
		DECLARE_DYNAMIC(ScreenScene)
	public:
		ScreenScene();
		virtual ~ScreenScene();
		BOOL Initialize(DX11& dx11, const TinyRectangle& s);
	public:
		BOOL Process(DX11& dx11) OVERRIDE;
		BOOL Render(DX11& dx11) OVERRIDE;
		void Clear(DX11& dx11) OVERRIDE;
	private:
		BYTE*			m_bits;
		HBITMAP			m_hBitmap;
		TinyRectangle	m_snapshot;
		DX11Duplicator	m_duplicator;
		TinyScopedPtr<TinyMemDC> m_memDC;
	};
}


