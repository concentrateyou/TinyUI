#pragma once
#include "DXFramework.h"
#include <d3d9.h>
#include <d3dx9tex.h>

namespace DXFramework
{
	class DX9RenderView;

	class DX9
	{
		friend class DX9Graphics2D;
		DISALLOW_COPY_AND_ASSIGN(DX9)
	public:
		DX9();
		virtual ~DX9();
		BOOL				Initialize(HWND hWND, INT cx, INT cy);
		BOOL				Present();
		BOOL				Resize(INT cx = 0, INT cy = 0);
		BOOL				SetViewport(const TinyPoint& pos, const TinySize& size);
		D3DXMATRIX*			GetMatrixs();
		void				SetMatrixs(const TinySize& size);
		BOOL				Reset();
		BOOL				CheckReason(HRESULT hRes);//获得设备移除原因
	public:
		HWND				GetHWND() const;
		BOOL				IsActive() const;
		BOOL				IsEmpty() const;
		IDirect3DDevice9*	GetD3D() const;
		TinySize			GetSize() const;
	private:
		void				SetRenderView(DX9RenderView* render2D);
		DX9RenderView*		GetRenderView() const;
	private:
		BOOL							m_bActive;
		HWND							m_hWND;
		TinySize						m_size;
		D3DPRESENT_PARAMETERS			m_d3dpp;
		D3DXMATRIX						m_matrixs[3];
		DX9RenderView*					m_render2D;
		TinyComPtr<IDirect3D9>			m_d3d9;
		TinyComPtr<IDirect3DDevice9>	m_d3dd9;
		TinyScopedPtr<DX9RenderView>	m_background2D;
	};
}

