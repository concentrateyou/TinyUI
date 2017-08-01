#include "stdafx.h"
#include "DX11ColorShader.h"

namespace DXFramework
{
	DX11ColorShader::DX11ColorShader()
	{
	}

	DX11ColorShader::~DX11ColorShader()
	{
	}
	BOOL DX11ColorShader::Initialize(DX11& dx11, const CHAR* vsFile, const CHAR* psFile)
	{
		HRESULT hRes = S_OK;
		TinyComPtr<ID3D10Blob> vertexShaderBuffer;
		TinyComPtr<ID3D10Blob> pixelShaderBuffer;
		D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
		D3D11_BUFFER_DESC matrixBufferDesc;
		D3D11_SAMPLER_DESC samplerDesc;
		wstring wvsFile = StringToWString(vsFile);
		hRes = D3DCompileFromFile(wvsFile.c_str(), NULL, NULL, "ColorVertexShader", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, NULL);
		if (hRes != S_OK)
			return FALSE;
		wstring wpsFile = StringToWString(psFile);
		hRes = D3DCompileFromFile(wpsFile.c_str(), NULL, NULL, "ColorPixelShader", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, NULL);
		if (hRes != S_OK)
			return FALSE;
		hRes = dx11.GetD3D()->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
		if (hRes != S_OK)
			return FALSE;
		hRes = dx11.GetD3D()->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
		if (hRes != S_OK)
			return FALSE;
		polygonLayout[0].SemanticName = "POSITION";
		polygonLayout[0].SemanticIndex = 0;
		polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		polygonLayout[0].InputSlot = 0;
		polygonLayout[0].AlignedByteOffset = 0;
		polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[0].InstanceDataStepRate = 0;

		polygonLayout[1].SemanticName = "COLOR";
		polygonLayout[1].SemanticIndex = 0;
		polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		polygonLayout[1].InputSlot = 0;
		polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[1].InstanceDataStepRate = 0;

		ULONG size = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
		hRes = dx11.GetD3D()->CreateInputLayout(polygonLayout, size, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
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
		return TRUE;
	}
	void DX11ColorShader::Render(DX11& dx11, INT indexCount, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix)
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
		dx11.GetImmediateContext()->VSSetShader(m_vertexShader, NULL, 0);
		dx11.GetImmediateContext()->PSSetShader(m_pixelShader, NULL, 0);
		dx11.GetImmediateContext()->PSSetSamplers(0, 1, &m_sampleState);
		dx11.GetImmediateContext()->IASetInputLayout(m_layout);
		dx11.GetImmediateContext()->DrawIndexed(indexCount, 0, 0);
	}
}

