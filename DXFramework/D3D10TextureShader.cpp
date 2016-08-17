#include "stdafx.h"
#include "D3D10TextureShader.h"

namespace DXFramework
{
	D3D10TextureShader::D3D10TextureShader()
		:m_techniquePtr(NULL),
		m_worldMatrixPtr(NULL),
		m_viewMatrixPtr(NULL),
		m_projectionMatrixPtr(NULL),
		m_texturePtr(NULL)
	{
	}
	D3D10TextureShader::~D3D10TextureShader()
	{
		m_techniquePtr = NULL;
		m_worldMatrixPtr = NULL;
		m_viewMatrixPtr = NULL;
		m_projectionMatrixPtr = NULL;
		m_texturePtr = NULL;
	}
	BOOL D3D10TextureShader::Initialize(ID3D10Device* pDevice, const string& name)
	{
		ASSERT(pDevice);
		HRESULT hRes;
		D3D10_INPUT_ELEMENT_DESC inputLayout[2];
		UINT numElements;
		D3D10_PASS_DESC passDesc;
		hRes = D3DX10CreateEffectFromFile(name.c_str(), NULL, NULL, "fx_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, pDevice, NULL, NULL, &m_effect, NULL, NULL);
		if (FAILED(hRes))
			return FALSE;
		m_techniquePtr = m_effect->GetTechniqueByName("TextureTechnique");
		if (!m_techniquePtr)
			return FALSE;
		inputLayout[0].SemanticName = "POSITION";
		inputLayout[0].SemanticIndex = 0;
		inputLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputLayout[0].InputSlot = 0;
		inputLayout[0].AlignedByteOffset = 0;
		inputLayout[0].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
		inputLayout[0].InstanceDataStepRate = 0;
		inputLayout[1].SemanticName = "TEXCOORD";
		inputLayout[1].SemanticIndex = 0;
		inputLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		inputLayout[1].InputSlot = 0;
		inputLayout[1].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
		inputLayout[1].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
		inputLayout[1].InstanceDataStepRate = 0;
		numElements = sizeof(inputLayout) / sizeof(inputLayout[0]);
		m_techniquePtr->GetPassByIndex(0)->GetDesc(&passDesc);
		hRes = pDevice->CreateInputLayout(inputLayout, numElements, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &m_layout);
		if (FAILED(hRes))
			return FALSE;
		m_worldMatrixPtr = m_effect->GetVariableByName("worldMatrix")->AsMatrix();
		m_viewMatrixPtr = m_effect->GetVariableByName("viewMatrix")->AsMatrix();
		m_projectionMatrixPtr = m_effect->GetVariableByName("projectionMatrix")->AsMatrix();
		m_texturePtr = m_effect->GetVariableByName("shaderTexture")->AsShaderResource();
		return TRUE;
	}
	void D3D10TextureShader::SetShaderParameters(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D10ShaderResourceView* texture)
	{
		if (m_worldMatrixPtr)
			m_worldMatrixPtr->SetMatrix((FLOAT*)&worldMatrix);
		if (m_viewMatrixPtr)
			m_viewMatrixPtr->SetMatrix((FLOAT*)&viewMatrix);
		if (m_projectionMatrixPtr)
			m_projectionMatrixPtr->SetMatrix((FLOAT*)&projectionMatrix);
		if (m_texturePtr)
			m_texturePtr->SetResource(texture);
	}
	void D3D10TextureShader::RenderShader(ID3D10Device* pDevice, int indexCount)
	{
		ASSERT(pDevice);
		D3D10_TECHNIQUE_DESC techniqueDesc;
		pDevice->IASetInputLayout(m_layout);
		m_techniquePtr->GetDesc(&techniqueDesc);
		for (INT i = 0; i < techniqueDesc.Passes; ++i)
		{
			m_techniquePtr->GetPassByIndex(i)->Apply(0);
			pDevice->DrawIndexed(indexCount, 0, 0);
		}
	}
	void D3D10TextureShader::Render(ID3D10Device* pDevice, INT indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D10ShaderResourceView* texture)
	{
		ASSERT(pDevice);
		SetShaderParameters(worldMatrix, viewMatrix, projectionMatrix, texture);
		RenderShader(pDevice, indexCount);
	}
}
