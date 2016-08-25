#include "stdafx.h"
#include "DX10TextureShader.h"

namespace DXFramework
{
	DX10TextureShader::DX10TextureShader()
		:m_worldMatrix(NULL),
		m_viewMatrix(NULL),
		m_projectionMatrix(NULL),
		m_shaderTexture(NULL),
		m_technique(NULL)
	{
	}
	DX10TextureShader::~DX10TextureShader()
	{
		m_technique = NULL;
		m_worldMatrix = NULL;
		m_viewMatrix = NULL;
		m_projectionMatrix = NULL;
		m_shaderTexture = NULL;
	}
	BOOL DX10TextureShader::LoadShader(const DX10& dx10, const CHAR* pzFile)
	{
		HRESULT hRes = S_OK;
		D3D10_INPUT_ELEMENT_DESC layout[2];
		D3D10_PASS_DESC desc;
		hRes = D3DX10CreateEffectFromFile(pzFile, NULL, NULL, "fx_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, dx10.GetD3D(), NULL, NULL, &m_effect, NULL, NULL);
		if (FAILED(hRes))
			return FALSE;
		m_technique = m_effect->GetTechniqueByName("TextureTechnique");
		if (!m_technique)
			return FALSE;
		layout[0].SemanticName = "POSITION";
		layout[0].SemanticIndex = 0;
		layout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		layout[0].InputSlot = 0;
		layout[0].AlignedByteOffset = 0;
		layout[0].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
		layout[0].InstanceDataStepRate = 0;
		layout[1].SemanticName = "TEXCOORD";
		layout[1].SemanticIndex = 0;
		layout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		layout[1].InputSlot = 0;
		layout[1].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
		layout[1].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
		layout[1].InstanceDataStepRate = 0;
		INT size = sizeof(layout) / sizeof(layout[0]);
		m_technique->GetPassByIndex(0)->GetDesc(&desc);
		hRes = dx10.GetD3D()->CreateInputLayout(layout, size, desc.pIAInputSignature, desc.IAInputSignatureSize, &m_layout);
		if (FAILED(hRes))
			return FALSE;
		m_worldMatrix = m_effect->GetVariableByName("worldMatrix")->AsMatrix();
		m_viewMatrix = m_effect->GetVariableByName("viewMatrix")->AsMatrix();
		m_projectionMatrix = m_effect->GetVariableByName("projectionMatrix")->AsMatrix();
		m_shaderTexture = m_effect->GetVariableByName("shaderTexture")->AsShaderResource();
		return TRUE;
	}
	void DX10TextureShader::Draw(const DX10& dx10, INT indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, DX10Texture* texture)
	{
		m_worldMatrix->SetMatrix((FLOAT*)&worldMatrix);
		m_viewMatrix->SetMatrix((FLOAT*)&viewMatrix);
		m_projectionMatrix->SetMatrix((FLOAT*)&projectionMatrix);
		m_shaderTexture->SetResource(texture->GetSRView());
		D3D10_TECHNIQUE_DESC desc;
		dx10.GetD3D()->IASetInputLayout(m_layout);
		m_technique->GetDesc(&desc);
		for (UINT i = 0; i < desc.Passes; ++i)
		{
			m_technique->GetPassByIndex(i)->Apply(0);
			dx10.GetD3D()->DrawIndexed(indexCount, 0, 0);
		}
	}
}
