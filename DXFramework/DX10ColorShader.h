#pragma once
#include "DX10Shader.h"

namespace DXFramework
{
	class DX10ColorShader : public DX10Shader
	{
		DISALLOW_COPY_AND_ASSIGN(DX10ColorShader)
	public:
		DX10ColorShader();
		virtual ~DX10ColorShader();
	public:
		BOOL	Initialize(DX10& dx10, const CHAR* vsFile, const CHAR* psFile) OVERRIDE;
		void	Render(DX10& dx10) OVERRIDE;
	public:
		void	SetShaderParameters(DX10& dx10, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, UINT indexs);
	protected:
		UINT							m_indexs;
		TinyComPtr<ID3D10Buffer>		m_buffer;
		TinyComPtr<ID3D10VertexShader>	m_vertexShader;
		TinyComPtr<ID3D10PixelShader>	m_pixelShader;
		TinyComPtr<ID3D10InputLayout>	m_layout;
	};
}


