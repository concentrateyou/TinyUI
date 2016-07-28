#pragma once
#include "D3DDevice.h"
#include "Render/TinyGDI.h"

enum GSColorFormat { GS_UNKNOWNFORMAT, GS_ALPHA, GS_GRAYSCALE, GS_RGB, GS_RGBA, GS_BGR, GS_BGRA, GS_RGBA16F, GS_RGBA32F, GS_B5G5R5A1, GS_B5G6R5, GS_R10G10B10A2, GS_DXT1, GS_DXT3, GS_DXT5 };
const UINT formatPitch[] = { 0, 1, 1, 4, 4, 4, 4, 8, 16, 0, 0, 0 };

namespace D3D
{
	class CD3D10Texture
	{
		friend class CD3DDevice;
	public:
		CD3D10Texture(CD3DDevice& device);
		~CD3D10Texture();
		BOOL IsValid() const;
		CD3DDevice* GetSystem();
		TinyUI::TinySize GetSize() const;
		BOOL CreateTexture(HANDLE hResource);
		BOOL CreateTexture(const SIZE& size, DXGI_FORMAT format, void *lpData, BOOL bGenMipMaps, BOOL bStatic);
		TinyUI::TinyComPtr<IDXGISurface1> GetSurface();
	private:
		CD3DDevice&										m_device;
		TinyUI::TinyComPtr<ID3D10Texture2D>				m_d3d10Texture2D;
		TinyUI::TinyComPtr<ID3D10ShaderResourceView>	m_d3d10SRView;
		TinyUI::TinyComPtr<ID3D10RenderTargetView>		m_d3dRenderTarget;
		TinyUI::TinySize								m_size;
	};
}
