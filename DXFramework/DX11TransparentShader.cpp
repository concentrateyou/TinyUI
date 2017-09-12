#include "stdafx.h"
#include "DX11TransparentShader.h"

namespace DXFramework
{
	DX11TransparentShader::DX11TransparentShader()
	{
	}
	DX11TransparentShader::~DX11TransparentShader()
	{
	}
	BOOL DX11TransparentShader::Initialize(DX11& dx11, const CHAR* vsFile, const CHAR* psFile)
	{
		HRESULT hRes = S_OK;
		TinyComPtr<ID3D10Blob> vertexShaderBuffer;
		TinyComPtr<ID3D10Blob> pixelShaderBuffer;
		D3D11_INPUT_ELEMENT_DESC layout[2];
		D3D11_BUFFER_DESC matrixBufferDesc = { 0 };
		D3D11_BUFFER_DESC transparentBufferDesc = { 0 };
		D3D11_SAMPLER_DESC samplerDesc;
		hRes = D3DX11CompileFromFile(vsFile, NULL, NULL, "TransparentVertexShader", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, NULL, NULL);
		if (hRes != S_OK)
			return FALSE;
		hRes = D3DX11CompileFromFile(psFile, NULL, NULL, "TransparentPixelShader", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer, NULL, NULL);
		if (hRes != S_OK)
			return FALSE;
		hRes = dx11.GetD3D()->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
		if (hRes != S_OK)
			return FALSE;
		hRes = dx11.GetD3D()->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
		if (hRes != S_OK)
			return FALSE;
		layout[0].SemanticName = "POSITION";
		layout[0].SemanticIndex = 0;
		layout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		layout[0].InputSlot = 0;
		layout[0].AlignedByteOffset = 0;
		layout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		layout[0].InstanceDataStepRate = 0;

		layout[1].SemanticName = "TEXCOORD";
		layout[1].SemanticIndex = 0;
		layout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		layout[1].InputSlot = 0;
		layout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		layout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		layout[1].InstanceDataStepRate = 0;

		ULONG size = sizeof(layout) / sizeof(layout[0]);
		hRes = dx11.GetD3D()->CreateInputLayout(layout, size, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
		if (hRes != S_OK)
			return FALSE;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.MipLODBias = 0.0F;
		samplerDesc.MaxAnisotropy = 0;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		hRes = dx11.GetD3D()->CreateSamplerState(&samplerDesc, &m_sampleState);
		if (hRes != S_OK)
			return FALSE;

		matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		matrixBufferDesc.ByteWidth = sizeof(MATRIXBUFFER);
		matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		matrixBufferDesc.MiscFlags = 0;
		matrixBufferDesc.StructureByteStride = 0;
		hRes = dx11.GetD3D()->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
		if (hRes != S_OK)
			return FALSE;

		transparentBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		transparentBufferDesc.ByteWidth = sizeof(TRANSPARENTBUFFER);
		transparentBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		transparentBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		transparentBufferDesc.MiscFlags = 0;
		transparentBufferDesc.StructureByteStride = 0;
		hRes = dx11.GetD3D()->CreateBuffer(&transparentBufferDesc, NULL, &m_transparentBuffer);
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}
	void DX11TransparentShader::Render(DX11& dx11, INT indexCount, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, DX11Texture2D* texture, FLOAT alpha)
	{
		XMMATRIX worldMatrix1 = XMMatrixTranspose(worldMatrix);
		XMMATRIX viewMatrix1 = XMMatrixTranspose(viewMatrix);
		XMMATRIX projectionMatrix1 = XMMatrixTranspose(projectionMatrix);
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (SUCCEEDED(dx11.GetImmediateContext()->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		{
			MATRIXBUFFER* dataPtr = (MATRIXBUFFER*)mappedResource.pData;
			dataPtr->world = worldMatrix1;
			dataPtr->view = viewMatrix1;
			dataPtr->projection = projectionMatrix1;
			dx11.GetImmediateContext()->Unmap(m_matrixBuffer, 0);
		}
		dx11.GetImmediateContext()->VSSetConstantBuffers(0, 1, &m_matrixBuffer);
		if (SUCCEEDED(dx11.GetImmediateContext()->Map(m_transparentBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		{
			TRANSPARENTBUFFER* dataPtr = (TRANSPARENTBUFFER*)mappedResource.pData;
			dataPtr->alpha = alpha;
			dx11.GetImmediateContext()->Unmap(m_matrixBuffer, 0);
		}
		dx11.GetImmediateContext()->VSSetConstantBuffers(0, 1, &m_transparentBuffer);
		ID3D11ShaderResourceView* pps = texture->GetSRView();
		dx11.GetImmediateContext()->PSSetShaderResources(0, 1, &pps);
		dx11.GetImmediateContext()->IASetInputLayout(m_layout);
		dx11.GetImmediateContext()->VSSetShader(m_vertexShader, NULL, 0);
		dx11.GetImmediateContext()->PSSetShader(m_pixelShader, NULL, 0);
		dx11.GetImmediateContext()->PSSetSamplers(0, 1, &m_sampleState);
		dx11.GetImmediateContext()->DrawIndexed(indexCount, 0, 0);
	}
}
