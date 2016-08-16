#pragma once
#include "Common.h"
#include "D3D10.h"
#include "D3D10Texture.h"
#include "D3D10TextureShader.h"

namespace DX
{
	class D3D10Graphics
	{
		DISALLOW_COPY_AND_ASSIGN(D3D10Graphics);
	public:
		D3D10Graphics();
		~D3D10Graphics();
		BOOL Initialize(HWND hWND, INT cx, INT cy);
	private:
		D3D10				m_d3d10;
		D3D10Texture		m_texture;
		D3D10TextureShader  m_textureShader;
		D3DCamera			m_camera;
	};
}

