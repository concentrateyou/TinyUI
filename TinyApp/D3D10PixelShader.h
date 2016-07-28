#pragma once
#include "D3DDevice.h"

namespace D3D
{
	class CD3D10PixelShader
	{
	public:
		CD3D10PixelShader();
		~CD3D10PixelShader();
	private:
		TinyComPtr<ID3D10PixelShader>	m_vertexShader;
		TinyComPtr<ID3D10InputLayout>	m_inputLayout;
	};
}

