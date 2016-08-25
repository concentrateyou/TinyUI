#pragma once
#include "DXFramework.h"
#include "DX10.h"
#include "DX10Texture.h"

namespace DXFramework
{
	/// <summary>
	/// ÎÆÀí×ÅÉ«Æ÷
	/// </summary>
	class DX10TextureShader
	{
		DISALLOW_COPY_AND_ASSIGN(DX10TextureShader)
	public:
		DX10TextureShader();
		~DX10TextureShader();
		BOOL Initialize(const DX10& dx10,const CHAR* pzFile);
		void Render(const DX10& dx10, INT indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, DX10Texture* pTexture);
	private:
		TinyComPtr<ID3D10Effect>			m_effect;
		TinyComPtr<ID3D10InputLayout>		m_layout;
		ID3D10EffectTechnique*				m_technique;
		ID3D10EffectMatrixVariable*			m_worldMatrix;
		ID3D10EffectMatrixVariable*			m_viewMatrix;
		ID3D10EffectMatrixVariable*			m_projectionMatrix;
		ID3D10EffectShaderResourceVariable* m_shaderTexture;
	};
}

