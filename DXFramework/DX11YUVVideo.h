#pragma once
#include "DX11ImageElement2D.h"
#include "Render/TinyGDI.h"
using namespace TinyUI;

namespace DXFramework
{
	class DX11YUVVideo : public DX11ImageElement2D
	{
		struct VERTEXTYPE
		{
			XMFLOAT3 position;
			XMFLOAT2 texture;
			XMFLOAT4 color;
		};
		DECLARE_DYNAMIC(DX11YUVVideo)
		DISALLOW_COPY_AND_ASSIGN(DX11YUVVideo)
	public:
		DX11YUVVideo();
		virtual ~DX11YUVVideo();
		BOOL			IsEmpty() const;
		BOOL			Create(DX11& dx11, INT cx, INT cy);
		void			Destory();
		BOOL			DrawImage(DX11& dx11);
		BOOL			Copy(DX11& dx11, const BYTE* pY, UINT strideY, const BYTE* pU, UINT strideU, const BYTE* pV, UINT strideV);
		DX11Texture2D*	GetTextureY();
		DX11Texture2D*	GetTextureU();
		DX11Texture2D*	GetTextureV();
	private:
		BOOL			Initialize(DX11& dx11);
		BOOL			Calculate(DX11& dx11);
	private:
		DX11Texture2D				m_textureY;
		DX11Texture2D				m_textureU;
		DX11Texture2D				m_textureV;
		VERTEXTYPE					m_vertexTypes[6];
		TinyComPtr<ID3D11Buffer>	m_vertex;
		TinyComPtr<ID3D11Buffer>	m_indexs;
	};
}


