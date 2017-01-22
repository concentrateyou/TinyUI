#pragma once
#include "DX11Element.h"
#include "Render/TinyGDI.h"
using namespace TinyUI;

namespace DXFramework
{
	class DX11Rectangle
	{
		struct VERTEXTYPE
		{
			D3DXVECTOR3 position;
			D3DXVECTOR4 color;
		};
	public:
		DX11Rectangle();
		virtual ~DX11Rectangle();
		BOOL Create(DX11& dx11, const TinySize& size);
		BOOL SetPosition(DX11& dx11, const TinyPoint& pos);
		virtual INT	GetIndexCount() const;
		BOOL Render(DX11& dx11);
	private:
		BOOL DX11Rectangle::Initialize(DX11& dx11);
	protected:
		TinyRectangle				m_rectangle;
		TinyComPtr<ID3D11Buffer>	m_vertexBuffer;
		TinyComPtr<ID3D11Buffer>	m_indexBuffer;
		TinyScopedArray<VERTEXTYPE> m_vertices;
	};
}


