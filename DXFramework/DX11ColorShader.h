#pragma once
#include "DXFramework.h"
#include "DX11.h"

namespace DXFramework
{
	class DX11ColorShader
	{
		DISALLOW_COPY_AND_ASSIGN(DX11ColorShader)
	public:
		DX11ColorShader();
		virtual ~DX11ColorShader();
		BOOL Initialize(DX11& dx11, const CHAR* vsFile, const CHAR* psFile);
		void Render(DX11& dx11, INT indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix);
	protected:
		TinyComPtr<ID3D11VertexShader>	m_vertexShader;
		TinyComPtr<ID3D11PixelShader>	m_pixelShader;
		TinyComPtr<ID3D11InputLayout>	m_layout;
		TinyComPtr<ID3D11SamplerState>	m_sampleState;
		TinyComPtr<ID3D11Buffer>		m_matrixBuffer;
	};
}



