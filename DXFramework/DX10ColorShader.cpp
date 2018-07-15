#include "stdafx.h"
#include "DX10ColorShader.h"

namespace DXFramework
{
	DX10ColorShader::DX10ColorShader()
		:m_indexs(0)
	{
	}


	DX10ColorShader::~DX10ColorShader()
	{
	}

	BOOL DX10ColorShader::Initialize(DX10& dx10, const CHAR* vsFile, const CHAR* psFile)
	{
		HRESULT hRes = S_OK;
		TinyComPtr<ID3D10Blob> errorMsg;
		TinyComPtr<ID3D10Blob> vertexShaderBuffer;
		TinyComPtr<ID3D10Blob> pixelShaderBuffer;
		D3D10_INPUT_ELEMENT_DESC layout[2];
		D3D10_BUFFER_DESC bufferDesc = { 0 };
		hRes = D3DCompileFromFile(StringToWString(vsFile).c_str(), NULL, NULL, "ColorVertexPixelShader", "vs_4_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMsg);
		if (hRes != S_OK)
		{
			CHAR* error = (CHAR*)errorMsg->GetBufferPointer();
			LOG(ERROR) << "DX11TextureShader Initialize - ColorVertexPixelShader: " << error;
			return FALSE;
		}
		hRes = D3DCompileFromFile(StringToWString(psFile).c_str(), NULL, NULL, "ColorPixelShader", "ps_4_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMsg);
		if (hRes != S_OK)
		{
			CHAR* error = (CHAR*)errorMsg->GetBufferPointer();
			LOG(ERROR) << "DX11TextureShader Initialize - ColorPixelShader: " << error;
			return FALSE;
		}
		hRes = dx10.GetD3D()->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_vertexShader);
		if (hRes != S_OK)
			return FALSE;
		hRes = dx10.GetD3D()->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), &m_pixelShader);
		if (hRes != S_OK)
			return FALSE;
		layout[0].SemanticName = "POSITION";
		layout[0].SemanticIndex = 0;
		layout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		layout[0].InputSlot = 0;
		layout[0].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
		layout[0].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
		layout[0].InstanceDataStepRate = 0;

		layout[1].SemanticName = "COLOR";
		layout[1].SemanticIndex = 0;
		layout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		layout[1].InputSlot = 0;
		layout[1].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
		layout[1].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
		layout[1].InstanceDataStepRate = 0;
		ULONG size = sizeof(layout) / sizeof(layout[0]);
		hRes = dx10.GetD3D()->CreateInputLayout(layout, size, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
		if (hRes != S_OK)
			return FALSE;
		bufferDesc.Usage = D3D10_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = sizeof(MATRIXBUFFER);
		bufferDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		hRes = dx10.GetD3D()->CreateBuffer(&bufferDesc, NULL, &m_buffer);
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}
	void DX10ColorShader::Render(DX10& dx10)
	{
		dx10.GetD3D()->IASetInputLayout(m_layout);
		dx10.GetD3D()->VSSetShader(m_vertexShader);
		dx10.GetD3D()->PSSetShader(m_pixelShader);
		dx10.GetD3D()->DrawIndexed(m_indexs, 0, 0);
	}
	void DX10ColorShader::SetShaderParameters(DX10& dx10, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, UINT indexs)
	{
		m_indexs = indexs;
		XMMATRIX worldMatrix1 = XMMatrixTranspose(worldMatrix);
		XMMATRIX viewMatrix1 = XMMatrixTranspose(viewMatrix);
		XMMATRIX projectionMatrix1 = XMMatrixTranspose(projectionMatrix);
		BYTE* bits = NULL;
		if (SUCCEEDED(m_buffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&bits)))
		{
			MATRIXBUFFER* dataPtr = (MATRIXBUFFER*)bits;
			dataPtr->world = worldMatrix1;
			dataPtr->view = viewMatrix1;
			dataPtr->projection = projectionMatrix1;
			m_buffer->Unmap();
		}
		dx10.GetD3D()->VSSetConstantBuffers(0, 1, &m_buffer);
	}
}

