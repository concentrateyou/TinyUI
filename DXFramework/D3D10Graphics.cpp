#include "stdafx.h"
#include "D3D10Graphics.h"

namespace DXFramework
{
	const FLOAT SCREEN_FAR = 1000.0f;
	const FLOAT SCREEN_NEAR = 0.1f;

	D3D10Graphics::D3D10Graphics()
	{

	}


	D3D10Graphics::~D3D10Graphics()
	{

	}

	BOOL D3D10Graphics::Initialize(HWND hWND, INT cx, INT cy)
	{
		if (m_d3d10.Initialize(hWND, cx, cy, SCREEN_FAR, SCREEN_NEAR))
		{
			m_camera.SetPosition(0.0f, 0.0F, -10.0F);
			return m_textureShader.Initialize(m_d3d10.GetDevice(), "texture.fx");
		}
		return FALSE;
	}
	ID3D10Device* D3D10Graphics::GetDevice() const
	{
		return m_d3d10.GetDevice();
	}
}