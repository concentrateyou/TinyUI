#pragma once
#include "D3DUtility.h"
using namespace TinyUI;

namespace D3D
{
	class CD3D10PixelShader
	{
		friend class CD3D10Device;
	public:
		CD3D10PixelShader();
		~CD3D10PixelShader();
		BOOL Compile(CD3D10Device* device, const TinyString& str);
	private:
		TinyComPtr<ID3D10PixelShader>	m_pixelShader;
		TinyComPtr<ID3D10Buffer>		m_constantBuffer;
	};
}

