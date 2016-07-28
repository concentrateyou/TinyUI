#include "stdafx.h"
#include "D3D10PixelShader.h"

namespace D3D
{
	CD3D10PixelShader::CD3D10PixelShader()
	{
	}


	CD3D10PixelShader::~CD3D10PixelShader()
	{
	}
	BOOL CD3D10PixelShader::Compile(CD3D10Device& device, const TinyString& str)
	{
		TinyComPtr<ID3D10Blob> shader;
		TinyComPtr<ID3D10Blob> errorMsgs;
		if (FAILED(D3DX10CompileFromMemory(str.STR(), str.GetSize(), NULL, NULL, NULL, TEXT("main"), TEXT("ps_4_0"), D3D10_SHADER_OPTIMIZATION_LEVEL3, 0, NULL, &shader, &errorMsgs, NULL)))
		{
			return FALSE;
		}
		if (FAILED(device.GetD3D()->CreatePixelShader(shader->GetBufferPointer(), shader->GetBufferSize(), &m_pixelShader)))
		{
			return FALSE;
		}
		INT	constantSize = 16;
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
