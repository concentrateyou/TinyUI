#pragma once
#include "DXFramework.h"
#include <d3d9.h>
#include <d3dx9tex.h>

namespace DXFramework
{
	class DX9Graphics2D;

	class DX9
	{
		friend class DX9Canvas;
		DISALLOW_COPY_AND_ASSIGN(DX9)
	public:
		DX9();
		virtual ~DX9();
		BOOL				Initialize(HWND hWND, const TinySize& size);
		BOOL				Initialize(HWND hWND, INT cx, INT cy);
		BOOL				Present();
		BOOL				Resize(INT cx, INT cy);
		BOOL				Resize(const TinySize& size);
		BOOL				SetViewport(const TinyPoint& pos, const TinySize& size);
		D3DXMATRIX*			GetMatrixs();
		void				SetMatrixs(const TinySize& size);
		BOOL				CheckReason(HRESULT hRes);//获得设备移除原因
	public:
		HWND				GetHWND() const;
		BOOL				IsActive() const;
		BOOL				IsEmpty() const;
		IDirect3DDevice9*	GetD3D() const;
		TinySize			GetSize() const;
	private:
		BOOL							m_bActive;
		HWND							m_hWND;
		TinySize						m_size;
		D3DPRESENT_PARAMETERS			m_d3dpp;
		D3DXMATRIX						m_matrixs[3];
		TinyComPtr<IDirect3D9>			m_d3d9;
		TinyComPtr<IDirect3DDevice9>	m_d3dd9;
	};
}

