#pragma once
#include "D3D10Device.h"
#include "D3D10Texture.h"
#include "D3D10TextureShader.h"
#include "D3D10Bitmap.h"

namespace DXFramework
{
	class D3D10Graphics
	{
		DISALLOW_COPY_AND_ASSIGN(D3D10Graphics);
	public:
		D3D10Graphics();
		~D3D10Graphics();
		BOOL Initialize(HWND hWND, INT cx, INT cy);
		ID3D10Device* GetDevice() const;
		BOOL Render(FLOAT rotation);
		BOOL Render();
	private:
		D3D10Device			m_d3d10;
		D3D10Texture		m_texture;
		D3D10TextureShader  m_textureShader;
		D3D10Bitmap			m_bitmap;
		D3DCamera			m_camera;
	};
}

