#pragma once
#include "DXFramework.h"

namespace DXFramework
{
	class D3D10TextureShader
	{
	public:
		D3D10TextureShader();
		~D3D10TextureShader();
		BOOL Initialize(ID3D10Device* pDevice, const string& name);
		void Render(ID3D10Device* pDevice, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D10ShaderResourceView*);
	private:
		void SetShaderParameters(D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D10ShaderResourceView*);
		void RenderShader(ID3D10Device* pDevice);
	private:
		ID3D10EffectTechnique*							m_techniquePtr;
		ID3D10EffectMatrixVariable*						m_worldMatrixPtr;
		ID3D10EffectMatrixVariable*						m_viewMatrixPtr;
		ID3D10EffectMatrixVariable*						m_projectionMatrixPtr;
		ID3D10EffectShaderResourceVariable*				m_texturePtr;
		TinyComPtr<ID3D10Effect>						m_effect;
		TinyComPtr<ID3D10InputLayout>					m_layout;
	};
}

