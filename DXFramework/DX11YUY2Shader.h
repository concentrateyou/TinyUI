#pragma once
#include "DX11Shader.h"

namespace DXFramework
{
	class DX11YUY2Shader : public DX11Shader
	{
		DISALLOW_COPY_AND_ASSIGN(DX11YUY2Shader)
	public:
		DX11YUY2Shader();
		virtual ~DX11YUY2Shader();
	public:
		BOOL	Initialize(DX11& dx11, const CHAR* vsFile, const CHAR* psFile) OVERRIDE;
		void	Render(DX11& dx11) OVERRIDE;
	public:
		void	SetShaderParameters(DX11& dx11, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, DX11Texture2D* textures[1]);
	protected:
		TinyComPtr<ID3D11Buffer>		m_buffer;
		TinyComPtr<ID3D11VertexShader>	m_vertexShader;
		TinyComPtr<ID3D11PixelShader>	m_pixelShader;
		TinyComPtr<ID3D11InputLayout>	m_layout;
		TinyComPtr<ID3D11SamplerState>	m_sampleState;
	};
}

