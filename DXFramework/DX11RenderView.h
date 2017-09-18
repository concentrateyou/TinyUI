#pragma once
#include "DX11Texture2D.h"

namespace DXFramework
{
	class DX11RenderView
	{
	public:
		DX11RenderView(DX11& dx11);
		virtual ~DX11RenderView();
		DX11&		GetDX11();
		TinySize	GetSize() const;
		BOOL		Create();
		BOOL		Create(INT cx, INT cy, BOOL bMap = TRUE, BOOL bSync = FALSE);
		BOOL		Resize();
		BOOL		Resize(INT cx, INT cy);
		void		BeginDraw();
		void		EndDraw();
		BOOL		SaveAs(const CHAR* pzName, D3DX11_IMAGE_FILE_FORMAT format);
		BYTE*		Map(DWORD& dwSize);
		void		Unmap();
		HANDLE		GetHandle() const;
		BOOL		Lock(UINT64 acqKey, DWORD dwMS);
		BOOL		Unlock(UINT64 relKey);
		ID3D11RenderTargetView* GetRTView() const;
		ID3D11DepthStencilView* GetDSView() const;
	protected:
		HANDLE								m_handle;
		FLOAT								m_aspect;//±ÈÀý
		DX11&								m_dx11;
		TinySize							m_size;
		TinyComPtr<IDXGIKeyedMutex>			m_mutex;
		TinyComPtr<ID3D11Texture2D>			m_depth2D;
		TinyComPtr<ID3D11DepthStencilView>	m_depthView;
		TinyComPtr<ID3D11Texture2D>			m_render2D;
		TinyComPtr<ID3D11RenderTargetView>	m_renderView;
		TinyComPtr<ID3D11Texture2D>			m_copy2D;
	};
}


