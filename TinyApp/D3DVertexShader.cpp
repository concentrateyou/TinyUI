#include "stdafx.h"
#include "D3DVertexShader.h"

namespace D3D
{
	const	INT	constantSize = 64;
	CD3DVertexShader::CD3DVertexShader()
	{
	}


	CD3DVertexShader::~CD3DVertexShader()
	{
	}
	BOOL CD3DVertexShader::Compile(CD3DDevice& device, const TinyString& str)
	{
		TinyComPtr<ID3D10Blob> shader;
		TinyComPtr<ID3D10Blob> errorMsgs;
		if (FAILED(D3DX10CompileFromMemory(str.STR(), str.GetSize(), NULL, NULL, NULL, TEXT("main"), TEXT("vs_4_0"), D3D10_SHADER_OPTIMIZATION_LEVEL3, 0, NULL, &shader, &errorMsgs, NULL)))
		{
			return FALSE;
		}
		if (FAILED(device.GetD3D()->CreateVertexShader(shader->GetBufferPointer(), shader->GetBufferSize(), &m_vertexShader)))
		{
			return FALSE;
		}
		TinyComPtr<ID3D10InputLayout> vShaderLayout;
		D3D10_INPUT_ELEMENT_DESC descs[] =
		{
			{ "SV_Position", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 }
		};
		if (FAILED(device.GetD3D()->CreateInputLayout(descs, 2, shader->GetBufferPointer(), shader->GetBufferSize(), &m_inputLayout)))
		{
			return FALSE;
		}
		D3D10_BUFFER_DESC dbd;
		::ZeroMemory(&dbd, sizeof(dbd));
		dbd.ByteWidth = (constantSize + 15) & 0xFFFFFFF0;
		dbd.Usage = D3D10_USAGE_DYNAMIC;
		dbd.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
		dbd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		if (FAILED(device.GetD3D()->CreateBuffer(&dbd, NULL, &m_constantBuffer)))
		{
			return FALSE;
		}
		return TRUE;
	}
}
