#pragma once
#include "DX11Element2D.h"
#include "DX11ColorShader.h"
using namespace TinyUI;

namespace DXFramework
{
	typedef struct tagLine2D
	{
		XMFLOAT2 pos;
		XMFLOAT4 color;
	}Line2D;

	class DX11Line2D : public DX11Element2D
	{
		struct VERTEXTYPE
		{
			XMFLOAT3 position;
			XMFLOAT2 texture;
			XMFLOAT4 color;
		};
		DECLARE_DYNAMIC(DX11Line2D)
		DISALLOW_COPY_AND_ASSIGN(DX11Line2D)
	public:
		DX11Line2D();
		virtual	~DX11Line2D();
		BOOL Create(DX11& dx11, Line2D* lines, INT count);
	public:
		BOOL Process(DX11& dx11) OVERRIDE;
	protected:
		TinyComPtr<ID3D11Buffer>	m_vertexs;
		TinyComPtr<ID3D11Buffer>	m_indexs;
		TinyScopedArray<VERTEXTYPE> m_vertices;
	};
}

