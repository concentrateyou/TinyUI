#pragma once
#include "D3DDevice.h"

namespace D3D
{
	class CD3DVertexShader
	{
	public:
		CD3DVertexShader();
		~CD3DVertexShader();
		BOOL Compile(CD3DDevice& device, const TinyString& str);
	private:
		TinyComPtr<ID3D10VertexShader>	m_vertexShader;
		TinyComPtr<ID3D10InputLayout>	m_inputLayout;
		TinyComPtr<ID3D10Buffer>		m_constantBuffer;
	};
}

