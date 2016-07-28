#pragma once
#include "D3D10Device.h"
using namespace TinyUI;

namespace D3D
{
	class CD3D10VertexShader
	{
		friend class CD3D10Device;
	public:
		CD3D10VertexShader();
		~CD3D10VertexShader();
		BOOL Compile(CD3D10Device& device, const TinyString& str);
	private:
		TinyComPtr<ID3D10VertexShader>	m_vertexShader;
		TinyComPtr<ID3D10InputLayout>	m_inputLayout;
		TinyComPtr<ID3D10Buffer>		m_constantBuffer;
	};
}

