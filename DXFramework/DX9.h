#pragma once
#include "DXFramework.h"
#include <d3d9.h>
#include <d3dx9tex.h>

namespace DXFramework
{
	class DX9RenderView;

	class DX9 : public TinyLock
	{
		friend class DX9RenderView;
		DISALLOW_COPY_AND_ASSIGN(DX9)
	public:
		DX9();
		virtual ~DX9();
		BOOL				Initialize(HWND hWND, INT cx, INT cy);
		BOOL				Present();
		BOOL				ResizeView(INT cx = 0, INT cy = 0);
		BOOL				SetViewport(const TinyPoint& pos, const TinySize& size);
		D3DXMATRIX*			GetMatrixs();
		void				SetMatrixs(const TinySize& size);
		void				SetRenderTexture2D(DX9RenderView* render2D);
		DX9RenderView*		GetRender2D() const;
		BOOL				Reset();
	public:
		HWND				GetHWND() const;
		BOOL				IsEmpty() const;
		IDirect3DDevice9*	GetD3D() const;
	private:
		HRESULT							m_status;
		HWND							m_hWND;
		D3DXMATRIX						m_matrixs[3];
		D3DVIEWPORT9					m_viewPort;
		DX9RenderView*					m_render2D;
		TinySize						m_size;
		TinyComPtr<IDirect3D9>			m_d3d9;
		TinyComPtr<IDirect3DDevice9>	m_d3dd9;
		TinyScopedPtr<DX9RenderView>	m_background2D;
	};
}

