#include "stdafx.h"
#include "DX11TextureShader.h"

namespace DXFramework
{
	DX11TextureShader::DX11TextureShader()
	{
	}
	DX11TextureShader::~DX11TextureShader()
	{
	}
	BOOL DX11TextureShader::Initialize(DX11& dx11, const CHAR* vsFile, const CHAR* psFile)
	{
		HRESULT hRes = S_OK;
		TinyComPtr<ID3D10Blob> errorMsg;
		TinyComPtr<ID3D10Blob> vertexShaderBuffer;
		TinyComPtr<ID3D10Blob> pixelShaderBuffer;
		D3D11_INPUT_ELEMENT_DESC layout[3];
		D3D11_BUFFER_DESC bufferDesc = { 0 };
		D3D11_SAMPLER_DESC samplerDesc;
		hRes = D3DX11CompileFromFile(vsFile, NULL, NULL, "TextureVertexShader", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, &errorMsg, NULL);
		if (hRes != S_OK)
		{
			CHAR* error = (CHAR*)errorMsg->GetBufferPointer();
			return FALSE;
		}
		hRes = D3DX11CompileFromFile(psFile, NULL, NULL, "TexturePixelShader", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer, &errorMsg, NULL);
		if (hRes != S_OK)
		{
			CHAR* error = (CHAR*)errorMsg->GetBufferPointer();
			return FALSE;
		}
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
		layout[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		layout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		layout[0].InstanceDataStepRate = 0;

		layout[1].SemanticName = "TEXCOORD";
		layout[1].SemanticIndex = 0;
		layout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		layout[1].InputSlot = 0;
		layout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		layout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		layout[1].InstanceDataStepRate = 0;

		layout[2].SemanticName = "COLOR";
		layout[2].SemanticIndex = 0;
		layout[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		layout[2].InputSlot = 0;
		layout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		layout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		layout[2].InstanceDataStepRate = 0;

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
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = sizeof(MATRIXBUFFER);
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		hRes = dx11.GetD3D()->CreateBuffer(&bufferDesc, NULL, &m_buffer);
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}
	void DX11TextureShader::Render(DX11& dx11, INT indexCount, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, DX11Texture2D* texture)
	{
		XMMATRIX worldMatrix1 = XMMatrixTranspose(worldMatrix);
		XMMATRIX viewMatrix1 = XMMatrixTranspose(viewMatrix);
		XMMATRIX projectionMatrix1 = XMMatrixTranspose(projectionMatrix);
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (SUCCEEDED(dx11.GetImmediateContext()->Map(m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		{
			MATRIXBUFFER* dataPtr = (MATRIXBUFFER*)mappedResource.pData;
			dataPtr->world = worldMatrix1;
			dataPtr->view = viewMatrix1;
			dataPtr->projection = projectionMatrix1;
			dx11.GetImmediateContext()->Unmap(m_buffer, 0);
		}
		dx11.GetImmediateContext()->VSSetConstantBuffers(0, 1, &m_buffer);
		ID3D11ShaderResourceView* pps = texture->GetSRView();
		dx11.GetImmediateContext()->IASetInputLayout(m_layout);
		dx11.GetImmediateContext()->VSSetShader(m_vertexShader, NULL, 0);
		dx11.GetImmediateContext()->PSSetShader(m_pixelShader, NULL, 0);
		dx11.GetImmediateContext()->PSSetSamplers(0, 1, &m_sampleState);
		dx11.GetImmediateContext()->PSSetShaderResources(0, 1, &pps);
		dx11.GetImmediateContext()->DrawIndexed(indexCount, 0, 0);
	}
}
