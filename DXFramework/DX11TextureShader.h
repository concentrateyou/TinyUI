#pragma once
#include "DXFramework.h"
#include "DX11.h"
#include "DX11Texture.h"
#include "DX11Shader.h"

namespace DXFramework
{
	/// <summary>
	/// ÎÆÀí×ÅÉ«Æ÷
	/// </summary>
	class DX11TextureShader : public DX11Shader
	{
		struct MATRIXBUFFER
		{
			D3DXMATRIX world;
			D3DXMATRIX view;
			D3DXMATRIX projection;
		};
		DISALLOW_COPY_AND_ASSIGN(DX11TextureShader)
	public:
		DX11TextureShader();
		virtual ~DX11TextureShader();
		BOOL Initialize(const DX11& dx11, const CHAR* vsFile, const CHAR* psFile);
		void Render(const DX11& dx11, INT indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, DX11Texture* pTexture);
	private:
		TinyComPtr<ID3D11VertexShader>	m_vertexShader;
		TinyComPtr<ID3D11PixelShader>	m_pixelShader;
		TinyComPtr<ID3D11InputLayout>	m_layout;
		TinyComPtr<ID3D11SamplerState>	m_sampleState;
		TinyComPtr<ID3D11Buffer>		m_matrixBuffer;
	};
}

