#pragma once
#include "DX11Element2D.h"
#include "DX11ColorShader.h"
using namespace TinyUI;

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
		BOOL Create(DX11& dx11, XMFLOAT2* points, DWORD count, XMFLOAT4 color);
		DWORD GetCount() const;
	public:
		BOOL Process(DX11& dx11) OVERRIDE;
	protected:
		DWORD						m_count;
		TinyComPtr<ID3D11Buffer>	m_vertexs;
		TinyComPtr<ID3D11Buffer>	m_indexs;
		TinyBuffer<VERTEXTYPE>		m_vertices;
	};
}

