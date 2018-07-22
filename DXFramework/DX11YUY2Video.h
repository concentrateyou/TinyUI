#pragma once
#include "DX11Texture2DElement.h"
#include "Render/TinyGDI.h"
using namespace TinyFramework;

namespace DXFramework
{
	/// <summary>
	/// YUY2 (packet)
	/// </summary>
	class DX11YUY2Video : public DX11Texture2DElement
	{
		struct VERTEXTYPE
		{
			XMFLOAT3 position;
			XMFLOAT2 texture;
			XMFLOAT4 color;
		};
		DECLARE_DYNAMIC(DX11YUY2Video)
		DISALLOW_COPY_AND_ASSIGN(DX11YUY2Video)
	public:
		DX11YUY2Video();
		virtual ~DX11YUY2Video();
		BOOL			IsEmpty() const;
		BOOL			Create(DX11& dx11, INT cx, INT cy);
		void			Destory();
		BOOL			Copy(DX11& dx11, const BYTE* bits, UINT stride);
		DX11Texture2D*	GetTexture();
	public:
		BOOL			DrawImage(DX11& dx11) OVERRIDE;
	private:
		BOOL			Initialize(DX11& dx11);
		BOOL			Calculate(DX11& dx11);
	private:
		DX11Texture2D				m_texture;
		VERTEXTYPE					m_vertexTypes[6];
		TinyComPtr<ID3D11Buffer>	m_vertex;
		TinyComPtr<ID3D11Buffer>	m_index;
	};
}


