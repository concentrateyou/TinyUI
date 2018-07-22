#pragma once
#include "DX11Element.h"
#include "DX11ColorShader.h"
using namespace TinyFramework;

namespace DXFramework
{
	class DX11Rectangle2D : public DX11Element
	{
		struct VERTEXTYPE
		{
			XMFLOAT3 position;
			XMFLOAT4 color;
		};
		DECLARE_DYNAMIC(DX11Rectangle2D)
		DISALLOW_COPY_AND_ASSIGN(DX11Rectangle2D)
	public:
		DX11Rectangle2D();
		virtual	~DX11Rectangle2D();
		BOOL	Create(DX11& dx11);
		void	Destory();
		DWORD	GetIndexs() const;
	public:
		BOOL	DrawRectangle(DX11& dx11, const XMFLOAT2 points[4], const XMFLOAT4& color);
		BOOL	FillRectangle(DX11& dx11, const XMFLOAT2 points[4], const XMFLOAT4& color);
	protected:
		DWORD						m_index;
		XMFLOAT2					m_points[4];
		VERTEXTYPE					m_vertexTypes[4];
		TinyComPtr<ID3D11Buffer>	m_vertex;
		TinyComPtr<ID3D11Buffer>	m_indexs[2];
	};
}

