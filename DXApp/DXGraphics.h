#pragma once
#include "DXFramework.h"
#include "DX11Image.h"
#include "DXCamera.h"
#include "DX11TextureShader.h"
using namespace DXFramework;

class DXGraphics : public TinyLock
{
public:
	DXGraphics();
	~DXGraphics();
	BOOL	Initialize(HWND hWND, INT cx, INT cy);
	BOOL	BeginScene();
	BOOL	EndScene();
	DX11&	GetD3D();
	BYTE*	GetPointer();
	BOOL	DrawImage(DX11Image& image, INT x, INT y);
private:
	BOOL	CreateTexture(INT cx, INT cy);
private:
	DX11							m_dx11;
	DX11TextureShader				m_textureShader;
	DXCamera						m_camera;
	TinyComPtr<ID3D11Resource>		m_resource;
	D3DXMATRIX						m_viewMatrix;
	D3DXMATRIX						m_worldMatrix;
	D3DXMATRIX						m_projectionMatrix;
	D3DXMATRIX						m_orthoMatrix;
	INT								m_cy;
	LONG							m_size;
	BYTE*							m_bits;
};

