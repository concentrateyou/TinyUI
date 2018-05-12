#pragma once
#include "DX11Element2D.h"
#include "DX11ColorShader.h"
using namespace TinyUI;

namespace DXFramework
{
	class DX11Rectangle2D : public DX11Element2D
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
		BOOL SetPrimitiveTopology(DX11& dx11, D3D11_PRIMITIVE_TOPOLOGY topology);
		BOOL Create(DX11& dx11, XMFLOAT2 points[4], XMFLOAT4 color);
		void Destory();
		BOOL SetColor(DX11& dx11, XMFLOAT4 color);
		BOOL SetPoints(DX11& dx11, XMFLOAT2 points[4]);
	public:
		DWORD GetIndexs() const;
	public:
		BOOL Process(DX11& dx11) OVERRIDE;
	protected:
		DWORD						m_indexs;
		XMFLOAT4					m_color;
		XMFLOAT2					m_points[4];
		D3D11_PRIMITIVE_TOPOLOGY	m_topology;
		TinyComPtr<ID3D11Buffer>	m_vertexBuffer;
		TinyComPtr<ID3D11Buffer>	m_indexBuffer;
		TinyScopedArray<VERTEXTYPE>	m_vertexes;
	};
}

