#include "stdafx.h"
#include "D3D10Graphics.h"
#include <io.h>
#include <iostream>

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
		if (!m_d3d10.Initialize(hWND, cx, cy, SCREEN_FAR, SCREEN_NEAR))
			return FALSE;
		m_camera.SetPosition(0.0f, 0.0F, -10.0F);

		CHAR szPath[MAX_PATH];
		GetModuleFileName(NULL, szPath, MAX_PATH);
		if (_tcsclen(szPath) > 0 && _taccess(szPath, 0) == 0)
		{
			TCHAR* pPath = _tcsrchr(szPath, _T('\\'));
			if (pPath)
			{
				*pPath = _T('\0');
				_tcscat_s(szPath, MAX_PATH, _T("\\texture.fx"));
			}
		}
		if (!m_textureShader.Initialize(m_d3d10.GetDevice(), szPath))
			return FALSE;
		if (!m_bitmap.Initialize(m_d3d10.GetDevice(), cx, cy, "D:\\test1.png"))
			return FALSE;
		return FALSE;
	}
	ID3D10Device* D3D10Graphics::GetDevice() const
	{
		return m_d3d10.GetDevice();
	}
	BOOL D3D10Graphics::Render()
	{
		static FLOAT rotation = 0.0F;
		rotation += (FLOAT)D3DX_PI * 0.005F;
		if (rotation > 360.0F)
		{
			rotation -= 360.0F;
		}
		return Render(rotation);
	}
	BOOL D3D10Graphics::Render(FLOAT rotation)
	{
		D3DXMATRIX worldMatrix;
		D3DXMATRIX viewMatrix;
		D3DXMATRIX projectionMatrix;
		D3DXMATRIX orthoMatrix;
		m_d3d10.BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
		m_camera.Render();
		m_camera.GetViewMatrix(viewMatrix);
		m_d3d10.GetWorldMatrix(worldMatrix);
		m_d3d10.GetProjectionMatrix(projectionMatrix);
		m_d3d10.GetOrthoMatrix(orthoMatrix);
		m_d3d10.AllowDepth(FALSE);
		if (!m_bitmap.Render(m_d3d10.GetDevice(), 0, 0))
			return FALSE;
		m_textureShader.Render(m_d3d10.GetDevice(), worldMatrix, viewMatrix, orthoMatrix, m_bitmap.GetTexture());
		m_d3d10.AllowDepth(TRUE);
		m_d3d10.EndScene();
		return TRUE;
	}
}