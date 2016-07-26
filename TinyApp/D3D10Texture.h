#pragma once
#include "D3DSystem.h"

namespace D3D
{
	class CD3D10Texture
	{
	public:
		CD3D10Texture(CD3DSystem* system);
		~CD3D10Texture();
		BOOL GetTexture(HANDLE hResource);
		TinyComPtr<IDXGISurface1> GetSurface();
	private:
		CD3DSystem*								m_system;
		TinyComPtr<ID3D10Texture2D>				m_d3d10Texture2D;
		TinyComPtr<ID3D10ShaderResourceView>	m_d3d10SRView;
		TinyComPtr<ID3D10RenderTargetView>		m_d3dRenderTarget;
	};
}
