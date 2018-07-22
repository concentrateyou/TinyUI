#pragma once
#include "DX11Image2D.h"
#include "IO/TinyIO.h"
#include "Render/TinyGDI.h"
using namespace TinyFramework;
using namespace TinyFramework::IO;

namespace DXFramework
{
	/// <summary>
	/// NV12 (2 planes)
	/// </summary>
	class DX11NV12Video : public DX11Texture2DElement
	{
		struct VERTEXTYPE
		{
			XMFLOAT3 position;
			XMFLOAT2 texture;
			XMFLOAT4 color;
		};
		DECLARE_DYNAMIC(DX11NV12Video)
		DISALLOW_COPY_AND_ASSIGN(DX11NV12Video)
	public:
		DX11NV12Video();
		virtual ~DX11NV12Video();
		BOOL			IsEmpty() const;
		BOOL			Create(DX11& dx11, INT cx, INT cy);
		void			Destory();
		BOOL			Copy(DX11& dx11, const BYTE* pY, UINT strideY, const BYTE* pUV, UINT strideUV);
		DX11Texture2D*	GetTextureY();
		DX11Texture2D*	GetTextureNV();
	public:
		BOOL			DrawImage(DX11& dx11) OVERRIDE;
	private:
		BOOL			Initialize(DX11& dx11);
		BOOL			Calculate(DX11& dx11);
	private:
		DX11Texture2D				m_textureY;
		DX11Texture2D				m_textureNV;
		VERTEXTYPE					m_vertexTypes[6];
		TinyComPtr<ID3D11Buffer>	m_vertex;
		TinyComPtr<ID3D11Buffer>	m_index;
	};
}


