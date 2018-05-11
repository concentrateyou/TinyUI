#pragma once
#include "DX11Shader.h"

namespace DXFramework
{
	/// <summary>
	/// ÎÆÀí×ÅÉ«Æ÷
	/// </summary>
	class DX11TextureShader : public DX11Shader
	{
		DISALLOW_COPY_AND_ASSIGN(DX11TextureShader)
	public:
		DX11TextureShader();
		virtual ~DX11TextureShader();
	public:
		BOOL	Initialize(DX11& dx11, const CHAR* vsFile, const CHAR* psFile) OVERRIDE;
		void	Render(DX11& dx11) OVERRIDE;
	public:
		void	SetShaderParameters(DX11& dx11, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, DX11Texture2D* texture2D);
	protected:
		TinyComPtr<ID3D11Buffer>		m_buffer;
		TinyComPtr<ID3D11VertexShader>	m_vertexShader;
		TinyComPtr<ID3D11PixelShader>	m_pixelShader;
		TinyComPtr<ID3D11InputLayout>	m_layout;
		TinyComPtr<ID3D11SamplerState>	m_sampleState;
	};
}

