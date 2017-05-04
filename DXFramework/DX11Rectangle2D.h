#pragma once
#include "DX11Element2D.h"
#include "Render/TinyGDI.h"
using namespace TinyUI;

namespace DXFramework
{
	class DX11Rectangle2D
	{
		struct VERTEXTYPE
		{
			XMFLOAT3 position;
			XMFLOAT4 color;
		};
	public:
		DX11Rectangle2D();
		virtual ~DX11Rectangle2D();
		BOOL	Create(DX11& dx11);
		BOOL	SetRectangle(DX11& dx11, const TinyRectangle& rectangle);
		BOOL	Render(DX11& dx11);
	public:
		virtual INT	GetIndexCount() const;
		virtual void Destory();
	private:
		BOOL Initialize(DX11& dx11);
	protected:
		TinyRectangle				m_rectangle;
		TinyComPtr<ID3D11Buffer>	m_vertexBuffer;
		TinyComPtr<ID3D11Buffer>	m_indexBuffer;
		TinyScopedArray<VERTEXTYPE> m_vertices;
	};
}


