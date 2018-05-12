#include "stdafx.h"
#include "DX11ColorShader.h"

namespace DXFramework
{
	DX11ColorShader::DX11ColorShader()
		:m_indexs(0)
	{
	}


	DX11ColorShader::~DX11ColorShader()
	{
	}

	BOOL DX11ColorShader::Initialize(DX11& dx11, const CHAR* vsFile, const CHAR* psFile)
	{
		HRESULT hRes = S_OK;
		TinyComPtr<ID3D10Blob> errorMsg;
		TinyComPtr<ID3D10Blob> vertexShaderBuffer;
		TinyComPtr<ID3D10Blob> pixelShaderBuffer;
		D3D11_INPUT_ELEMENT_DESC layout[2];
		D3D11_BUFFER_DESC bufferDesc = { 0 };
		hRes = D3DX11CompileFromFile(vsFile, NULL, NULL, "ColorVertexPixelShader", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, &errorMsg, NULL);
		if (hRes != S_OK)
		{
			CHAR* error = (CHAR*)errorMsg->GetBufferPointer();
			LOG(ERROR) << "DX11TextureShader Initialize - ColorVertexPixelShader: " << error;
			return FALSE;
		}
		hRes = D3DX11CompileFromFile(psFile, NULL, NULL, "ColorPixelShader", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer, &errorMsg, NULL);
		if (hRes != S_OK)
		{
			CHAR* error = (CHAR*)errorMsg->GetBufferPointer();
			LOG(ERROR) << "DX11TextureShader Initialize - ColorPixelShader: " << error;
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

		layout[1].SemanticName = "COLOR";
		layout[1].SemanticIndex = 0;
		layout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		layout[1].InputSlot = 0;
		layout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		layout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		layout[1].InstanceDataStepRate = 0;
		ULONG size = sizeof(layout) / sizeof(layout[0]);
		hRes = dx11.GetD3D()->CreateInputLayout(layout, size, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
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
	void DX11ColorShader::Render(DX11& dx11)
	{
		dx11.GetImmediateContext()->IASetInputLayout(m_layout);
		dx11.GetImmediateContext()->VSSetShader(m_vertexShader, NULL, 0);
		dx11.GetImmediateContext()->PSSetShader(m_pixelShader, NULL, 0);
		dx11.GetImmediateContext()->DrawIndexed(m_indexs, 0, 0);
	}
	void DX11ColorShader::SetShaderParameters(DX11& dx11, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, UINT indexs)
	{
		m_indexs = indexs;
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
	}
}

