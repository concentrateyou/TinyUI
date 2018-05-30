#pragma once
#include "DX11Element2D.h"
#include "DX11ColorShader.h"
using namespace TinyFramework;

namespace DXFramework
{
	class DX11Line2D : public DX11Element2D
	{
		struct VERTEXTYPE
		{
			XMFLOAT3 position;
			XMFLOAT4 color;
		};
		DECLARE_DYNAMIC(DX11Line2D)
		DISALLOW_COPY_AND_ASSIGN(DX11Line2D)
	public:
		DX11Line2D();
		virtual	~DX11Line2D();
		BOOL	Create(DX11& dx11);
		void	Destory();
		DWORD	GetIndexs() const;
		BOOL	DrawLine(DX11& dx11, const XMFLOAT2* points, DWORD count, const XMFLOAT4& color);
	protected:
		TinyBuffer<VERTEXTYPE>		m_vertexTypes;
		TinyComPtr<ID3D11Buffer>	m_vertex;
		TinyComPtr<ID3D11Buffer>	m_index;
	};
}

