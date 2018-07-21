#include "stdafx.h"
#include "DX10TextureShader.h"

namespace DXFramework
{
	DX10TextureShader::DX10TextureShader()
	{
	}
	DX10TextureShader::~DX10TextureShader()
	{
	}

	BOOL DX10TextureShader::Initialize(DX10& dx10, const CHAR* vsFile, const CHAR* psFile)
	{
		HRESULT hRes = S_OK;
		TinyComPtr<ID3D10Blob> errorMsg;
		TinyComPtr<ID3D10Blob> vertexShaderBuffer;
		TinyComPtr<ID3D10Blob> pixelShaderBuffer;
		D3D10_INPUT_ELEMENT_DESC layout[3];
		D3D10_BUFFER_DESC bufferDesc = { 0 };
		D3D10_SAMPLER_DESC samplerDesc;
		hRes = D3DCompileFromFile(StringToWString(vsFile).c_str(), NULL, NULL, "VS_MAIN", "vs_4_0_level_9_3", D3DCOMPILE_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMsg);
		if (hRes != S_OK)
		{
			CHAR* error = (CHAR*)errorMsg->GetBufferPointer();
			LOG(ERROR) << "DX10TextureShader Initialize - TextureVertexShader: " << error;
			return FALSE;
		}
		hRes = D3DCompileFromFile(StringToWString(psFile).c_str(), NULL, NULL, "PS_MAIN", "ps_4_0_level_9_3", D3DCOMPILE_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMsg);
		if (hRes != S_OK)
		{
			CHAR* error = (CHAR*)errorMsg->GetBufferPointer();
			LOG(ERROR) << "DX10TextureShader Initialize - TexturePixelShader: " << error;
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

		layout[1].SemanticName = "TEXCOORD";
		layout[1].SemanticIndex = 0;
		layout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		layout[1].InputSlot = 0;
		layout[1].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
		layout[1].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
		layout[1].InstanceDataStepRate = 0;

		layout[2].SemanticName = "COLOR";
		layout[2].SemanticIndex = 0;
		layout[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		layout[2].InputSlot = 0;
		layout[2].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
		layout[2].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
		layout[2].InstanceDataStepRate = 0;

		ULONG size = sizeof(layout) / sizeof(layout[0]);
		hRes = dx10.GetD3D()->CreateInputLayout(layout, size, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
		if (hRes != S_OK)
			return FALSE;
		samplerDesc.Filter = D3D10_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D10_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D10_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D10_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.MipLODBias = 0.0F;
		samplerDesc.MaxAnisotropy = 0;
		samplerDesc.ComparisonFunc = D3D10_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D10_FLOAT32_MAX;
		hRes = dx10.GetD3D()->CreateSamplerState(&samplerDesc, &m_sampleState);
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
	void DX10TextureShader::Render(DX10& dx10)
	{
		dx10.GetD3D()->IASetInputLayout(m_layout);
		dx10.GetD3D()->VSSetShader(m_vertexShader);
		dx10.GetD3D()->PSSetShader(m_pixelShader);
		dx10.GetD3D()->PSSetSamplers(0, 1, &m_sampleState);
		dx10.GetD3D()->DrawIndexed(6, 0, 0);
	}
	void DX10TextureShader::SetShaderParameters(DX10& dx10, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, DX10Texture2D* texture2D)
	{
		ASSERT(m_buffer);
		if (texture2D != NULL)
		{
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
			ID3D10ShaderResourceView* spv = texture2D->GetSRView();
			ASSERT(spv);
			dx10.GetD3D()->PSSetShaderResources(0, 1, &spv);
		}

	}
}
