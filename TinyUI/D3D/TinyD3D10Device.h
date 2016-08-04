#pragma once
#include "../Common/TinyCommon.h"
#include "../Render/TinyGDI.h"
#include "../Common/TinyString.h"
#include "TinyD3D10Common.h"
#include <string>
using namespace std;

namespace TinyUI
{
	namespace D3D
	{
		/// <summary>
		/// D3D10设备
		/// </summary>
		class TinyD3D10Device
		{
		public:
			BOOL Initialize(HWND hWND, INT cx, INT cy);
			void SetViewports(INT x, INT y, INT cx, INT cy);
			BOOL Render();
			ID3D10Device1*	GetDevice();
			BOOL LoadTexture(LPCSTR pzFile, ID3D10Texture2D** texture2D);
			BOOL GetResourceView(ID3D10Texture2D* texture2D, ID3D10ShaderResourceView **resourceView);
		private:
			TinyComPtr<IDXGISwapChain>				m_swap;
			TinyComPtr<ID3D10Device1>				m_d3d;
			TinyComPtr<ID3D10RenderTargetView>		m_renderView;
			TinyComPtr<ID3D10DepthStencilState>		m_depthState;
			TinyComPtr<ID3D10RasterizerState>		m_rasterizerState;
			TinyComPtr<ID3D10BlendState>			m_blendState;
			TinyComPtr<ID3D10ShaderResourceView>	m_resourceView;
		};
	};
}

