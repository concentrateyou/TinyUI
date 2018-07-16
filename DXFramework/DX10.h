#pragma once
#include "DXFramework.h"

namespace DXFramework
{
	class DX10Graphics2D;

	class DX10
	{
		friend class DX10Graphics2D;
		DISALLOW_COPY_AND_ASSIGN(DX10)
	public:
		DX10();
		virtual ~DX10();
		BOOL					EnumAdapters(vector<IDXGIAdapter1*>& adapters);
		BOOL					Initialize(HWND hWND, INT cx, INT cy, IDXGIAdapter1* pAdapter = NULL);
		void					Uninitialize();
		BOOL					Resize(INT cx, INT cy);
		BOOL					SetViewport(const TinyPoint& pos, const TinySize& size);
		BOOL					AllowBlend(BOOL bAllow, FLOAT blendFactor[4]);
		BOOL					AllowDepth(BOOL bAllow);
		BOOL					Present();
		BOOL					Flush();
		void					SetMatrixs(const TinySize& size);
	public:
		HWND					GetHWND() const;
		BOOL					IsEmpty() const;
		ID3D10Device*			GetD3D() const;
		IDXGISwapChain*			GetSwapChain() const;
		XMMATRIX*				GetMatrixs();
		TinySize				GetSize() const;
	private:
		HWND								m_hWND;
		XMMATRIX							m_matrixs[3];
		TinySize							m_size;
		TinyComPtr<ID3D10Device1>			m_d3d;
		TinyComPtr<IDXGISwapChain>			m_swap;
		TinyComPtr<ID3D10RasterizerState>	m_rasterizerState;
		TinyComPtr<ID3D10BlendState>		m_enableBlendState;
		TinyComPtr<ID3D10BlendState>		m_disableBlendState;
		TinyComPtr<ID3D10DepthStencilState>	m_enableDepthState;
		TinyComPtr<ID3D10DepthStencilState>	m_disableDepthState;
	};
}

