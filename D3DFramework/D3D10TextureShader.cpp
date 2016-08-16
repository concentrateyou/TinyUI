#include "stdafx.h"
#include "D3D10TextureShader.h"

namespace DX
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
	BOOL D3D10TextureShader::Initialize(ID3D10Device* device, string name)
	{
		HRESULT hRes;
		D3D10_INPUT_ELEMENT_DESC polygonLayout[2];
		UINT numElements;
		D3D10_PASS_DESC passDesc;
		hRes = D3DX10CreateEffectFromFile(name.c_str(), NULL, NULL, "fx_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, device, NULL, NULL, &m_effect, NULL, NULL);
		if (FAILED(hRes))
			return FALSE;
		m_techniquePtr = m_effect->GetTechniqueByName("TextureTechnique");
		if (!m_techniquePtr)
			return FALSE;
		polygonLayout[0].SemanticName = "POSITION";
		polygonLayout[0].SemanticIndex = 0;
		polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		polygonLayout[0].InputSlot = 0;
		polygonLayout[0].AlignedByteOffset = 0;
		polygonLayout[0].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
		polygonLayout[0].InstanceDataStepRate = 0;

		polygonLayout[1].SemanticName = "TEXCOORD";
		polygonLayout[1].SemanticIndex = 0;
		polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		polygonLayout[1].InputSlot = 0;
		polygonLayout[1].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
		polygonLayout[1].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
		polygonLayout[1].InstanceDataStepRate = 0;
		numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
		m_techniquePtr->GetPassByIndex(0)->GetDesc(&passDesc);
		hRes = device->CreateInputLayout(polygonLayout, numElements, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &m_layout);
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
		m_worldMatrixPtr->SetMatrix((FLOAT*)&worldMatrix);
		m_viewMatrixPtr->SetMatrix((FLOAT*)&viewMatrix);
		m_projectionMatrixPtr->SetMatrix((FLOAT*)&projectionMatrix);
		m_texturePtr->SetResource(texture);
	}
	void D3D10TextureShader::RenderShader(ID3D10Device* device, int indexCount)
	{
		D3D10_TECHNIQUE_DESC techniqueDesc;
		device->IASetInputLayout(m_layout);
		m_techniquePtr->GetDesc(&techniqueDesc);
		for (INT i = 0; i < techniqueDesc.Passes; ++i)
		{
			m_techniquePtr->GetPassByIndex(i)->Apply(0);
			device->DrawIndexed(indexCount, 0, 0);
		}
	}
	void D3D10TextureShader::Render(ID3D10Device* device, INT indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D10ShaderResourceView* texture)
	{
		SetShaderParameters(worldMatrix, viewMatrix, projectionMatrix, texture);
		RenderShader(device, indexCount);
	}
}
