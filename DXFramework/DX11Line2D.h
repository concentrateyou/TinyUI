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
		DWORD GetIndexs() const;
	public:
		BOOL Process(DX11& dx11) OVERRIDE;
	protected:
		DWORD						m_indexs;
		TinyComPtr<ID3D11Buffer>	m_vertexBuffer;
		TinyComPtr<ID3D11Buffer>	m_indexBuffer;
		TinyBuffer<VERTEXTYPE>		m_vertices;
	};
}

